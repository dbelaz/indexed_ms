//
//  CmdArguments.h
//  fast_ms
//
//  Created by denas on 11/21/16.
//  Copyright © 2016 denas. All rights reserved.
//

#ifndef CmdArguments_h
#define CmdArguments_h

// copied from http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
class InputParser{
public:
    std::string empty = "";
    InputParser (int &argc, char **argv){
        for (int i=1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /// @author iain
    const std::string& getCmdOption(const std::string &option) const{
        std::vector<std::string>::const_iterator itr;
        itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()){
            return *itr;
        }
        return empty;
    }
    /// @author iain
    bool cmdOptionExists(const std::string &option) const{
        return std::find(this->tokens.begin(), this->tokens.end(), option)
        != this->tokens.end();
    }
private:
    std::vector <std::string> tokens;
};

#endif /* CmdArguments_h */
