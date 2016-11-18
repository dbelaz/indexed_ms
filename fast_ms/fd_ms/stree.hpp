//
//  stree.h
//  fast_ms
//
//  Created by denas on 11/12/16.
//  Copyright © 2016 denas. All rights reserved.
//

#ifndef stree_h
#define stree_h

#include "bp.hpp"
#include "fd_ms.hpp"

using namespace std;


namespace fdms {

typedef uint8_t       char_type;
typedef unsigned long size_type;
typedef size_type     node_type;


/*
// Gets ISA[SA[idx]+d]
// d = depth of the character 0 = first position
size_type get_char_pos(size_type idx, size_type d, const t_csa& csa) {
    if (d == 0)
        return idx;
    // if we have to apply \f$\LF\f$ or \f$\Phi\f$ more
    // than 2*d times to calc csa(csa[idx]+d), we opt to
    // apply \f$ \Phi \f$ d times
    if (csa.sa_sample_dens + csa.isa_sample_dens > 2*d+2) {
        for (size_type i=0; i < d; ++i)
            idx = csa.psi[idx];
        return idx;
    }
    return csa.isa[csa[idx] + d];
}
*/
class Stree{
private:
    fdms::bp_support_sada<>&       m_bp_supp;
    sdsl::rank_support_v5<10,2>    m_bp_rank10;
    sdsl::select_support_mcl<10,2> m_bp_select10;
    Bwt& m_bwt;

public:
    Stree(fdms::bp_support_sada<>& bp_supp, Bwt& bwt) : m_bwt{bwt}, m_bp_supp{bp_supp} {
        util::init_support(m_bp_rank10, m_bp_supp.m_bp);
        util::init_support(m_bp_select10, m_bp_supp.m_bp);
    }

    node_type root() const { return 0; }

    bool is_leaf(node_type v) const {
        assert((*m_bp_supp.m_bp)[v] == 1);  // assert v is a valid node
        // if there is a closing parenthesis at position v+1, the node is a leaf
        return !(*m_bp_supp.m_bp)[v + 1];
    }

    node_type parent(node_type v) const {
        assert((*m_bp_supp.m_bp)[v] == 1); //assert valid node
        if(v == root())
            return root();
        return m_bp_supp.enclose(v);
    }

    size_type depth(node_type v) const {
        // -2 as the root() we assign depth=0 to the root
        return (m_bp_supp.rank(v)<<1)-v-2;
    }

    //!Calculates the index of the leftmost leaf in the corresponding suffix array.
    /*!\param v A valid node of the suffix tree.
     * \return The index of the leftmost leaf in the corresponding suffix array.
     * \par Time complexity
     *  \f$ \Order{1} \f$
     * \par Note
     * lb is an abbreviation for ,,left bound''
     */
    size_type lb(const node_type v) const { return m_bp_rank10(v); }

    //! Calculates the index of the rightmost leaf in the corresponding suffix array.
    /*! \param v A valid node of the suffix tree.
     *  \return The index of the rightmost leaf in the corresponding suffix array.
     *  \par Time complexity
     *    \f$ \Order{1} \f$
     *  \par Note
     *   rb is an abbreviation for ,,right bound''
     */
    size_type rb(const node_type v) const
    {
        size_type r = m_bp_supp.find_close(v);
        return m_bp_rank10(r + 1) - 1;
    }


    // Gets ISA[SA[idx]+d]
    // d = depth of the character 0 = first position
    size_type get_char_pos(size_type idx, size_type d) const {
        for(size_type i = 0; i < d; i++)
            idx =  m_bwt.lf_rev(idx);
        return idx;
    }

    /* Get the number of leaves that are in the subtree rooted at the first child of v +
     * number of leafs in the subtrees rooted at the children of parent(v) which precede v in the tree.
     */
    size_type inorder(node_type v) const {
        return m_bp_rank10(m_bp_supp.find_close(v + 1) + 1);
    }

    node_type child(node_type v, const char_type c, size_type& char_pos) const {
        if (is_leaf(v))  // if v is a leaf = (), v has no child
            return root();
        // else v = ( (     ))
        size_type cc = m_bwt.char2int[c];
        //if (cc == 0 and c != 0) // TODO: aendere char2comp so ab, dass man diesen sonderfall nicht braucht
        //    return root();
        size_type char_ex_max_pos = m_bwt.C[cc + 1], char_inc_min_pos = m_bwt.C[cc];

        size_type d = depth(v);
        size_type res = v + 1;
        while (true) {
            if (is_leaf(res)) {
                char_pos = get_char_pos(m_bp_rank10(res), d);
            } else {
                char_pos = get_char_pos(inorder(res), d);
            }
            if (char_pos >= char_ex_max_pos)  // if the current char is lex. greater than the searched char: exit
                return root();
            if (char_pos >= char_inc_min_pos)  // if the current char is lex. equal with the
                return res;
            res = m_bp_supp.find_close(res) + 1;
            if (!(*m_bp_supp.m_bp)[res]) // closing parenthesis: there exists no next child
                return root();
        }
    }

    //! Get the child w of node v which edge label (v,w) starts with character c.
    // \sa child(node_type v, const char_type c, size_type &char_pos)
    node_type child(node_type v, const char_type c) const {
        size_type char_pos;
        return child(v, c, char_pos);
    }


    node_type select_leaf(size_type i) const {
        assert(i > 0 and i <= m_bwt.bwt_len);
        // -1 as select(i) returns the postion of the 0 of pattern 10
        return m_bp_select10.select(i) - 1;
    }

    //! Calculate the lowest common ancestor (lca) of two nodes v and w of the suffix tree.
    /*!
     * \param v The first node for which the lca with the second node should be computed.
     * \param w The second node for which the lca with the first node should be computed.
     * \return A node that is the lowest common ancestor of v and w in the suffix tree.
     * \par Time complexity
     *    \f$ \Order{\rrenclose}\   \f$
     */
    node_type lca(node_type v, node_type w)const
    {
        assert((*m_bp_supp.m_bp)[v] == 1 and (*m_bp_supp.m_bp)[w] == 1);
        if (v > w)
            std::swap(v, w);
        else if (v == w)
            return v;

        if (v == root())
            return root();
        return m_bp_supp.double_enclose(v, w);
    }

    //! Compute the Weiner link of node v and character c.
    /*
     * \param v A valid node of a cst_sada.
     * \param c The character which should be prepended to the string of the current node.
     *   \return root() if the Weiner link of (v, c) does not exist, otherwise the Weiner link is returned.
     * \par Time complexity
     *    \f$ \Order{ t_{rank\_bwt} + t_{lca}}\f$
     */
    node_type wl(node_type v, const char_type c) const {
        // get leftmost leaf in the tree rooted at v
        size_type left        = m_bp_rank10(v);
        // get the rightmost leaf in the tree rooted at v
        size_type right = is_leaf(v) ? left : m_bp_rank10(m_bp_supp.find_close(v)) - 1;

        size_type c_left    = m_bwt.rank(left, c);
        size_type c_right    = m_bwt.rank(right + 1, c);

        if (c_left == c_right)  // there exists no Weiner link
            return root();

        if (c_left + 1 == c_right)
            return select_leaf(m_bwt.C[m_bwt.char2int[c]] + c_left + 1);
        else {
            size_type left    = m_bwt.C[m_bwt.char2int[c]] + c_left;
            size_type right    = m_bwt.C[m_bwt.char2int[c]] + c_right - 1;
            assert(left < right);
            node_type left_leaf = select_leaf(left+1);
            node_type right_leaf= select_leaf(right+1);
            return lca(left_leaf, right_leaf);
        }
    }

    //! Compute the suffix link of node v.
    /*!
     * \param v A valid node of a cst_sada.
     * \return The suffix link of node v.
     * \par Time complexity
     *   \f$ \Order{ 1 } \f$
     */
    node_type sl(node_type v)const
    {
        if (v == root())
            return root();
        // get leftmost leaf in the tree rooted at v
        size_type left        = m_bp_rank10(v);
        if (is_leaf(v)) {
            return select_leaf(m_bwt.lf_rev(left) + 1);
        }
        // get the rightmost leaf in the tree rooted at v
        size_type right = m_bp_rank10(m_bp_supp.find_close(v)) - 1;
        assert(left < right);
        node_type left_leaf = select_leaf(m_bwt.lf_rev(left) + 1);
        node_type right_leaf= select_leaf(m_bwt.lf_rev(right) + 1);
        return lca(left_leaf, right_leaf);
    }

};

}

#endif /* stree_h */