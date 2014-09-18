///
///  Interpreter.hpp
///  is part of CES Library.
///
///  C++ Embedded Script - CES is used to write template file for code generation.
///  More information about CES at <https://github.com/trfd/CES>
///
///  Created by Baptiste Dupy on 12/09/2014.
///  Contact:
///          Mail:       <baptiste.dupy@gmail.com>
///          GitHub:     trfd <https://github.com/trfd>
///          Website:    <http://www.trf-d.com>
///
///  Copyright (C) 2014 Baptiste (trfd) Dupy
///
///  This program is free software: you can redistribute it and/or modify
///  it under the terms of the GNU General Public License as published by
///  the Free Software Foundation, either version 3 of the License, or
///  (at your option) any later version.
///
///  This program is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///  GNU General Public License for more details.
///
///  You should have received a copy of the GNU General Public License
///  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CES_Interpreter_hpp
#define CES_Interpreter_hpp

#include <sstream>

#include "AST.hpp"
#include "CESIO.hpp"

namespace ces
{
    class LuaVM;
    
    /// <summary>
    ///
    /// </summary>
    class Interpreter
    {
    public:
        
        typedef std::shared_ptr<AST> AST_ptr;
        typedef std::ostringstream   ScriptStream;
        
        /// <summary>
        /// Interpret the content of the AST and stream the output.
        /// </summary>
        /// This method goes through the entire AST.
        /// Whenever it encounters source code interpreter streams it to the output stream
        /// When it encounters lua code interpreter send it to the LuaVM.
        /// and when it encounters "Out" code it firstly sends it to the vm
        /// then output the result.
        template<typename _OutputStream>
        void interpret(LuaVM* lvm_, AST_ptr ast_, _OutputStream& stream_)
        {
            if(!ast_->root() || ast_->root()->kind != Node::Block)
                return;
        
            //TODO: Generate proper IDs
            unsigned sessionID = rand();
            
            ScriptStream script;
            
            createScript(sessionID, ast_, script);
            
            runScript(sessionID, lvm_, script, stream_);
        }
        
        /// <summary>
        /// Visits the AST and creates the corresponding Lua script.
        /// </summary>
        /// This method is called by Interpreter::interpret.
        /// The ScriptStream passed is filled, passing a custom instance of ScriptStream
        /// allow to get the generated script back for further modification.
        /// The the script should be run using Interpreter::runScript as it creates
        /// and register an InterpreterSession to receive the calls "CES.out" from
        /// Lua.
        void createScript(unsigned sessionID_, AST_ptr ast_, ScriptStream& script_)
        {
            /// Add the session ID to the script
            script_<<"sessionID="<<sessionID_<<"\n";
            
            /// Initialize AST visiting
            Block* currentBlock = static_cast<Block*>(ast_->root());
            std::vector<unsigned> indexes = {0};
            
            while(indexes.size() > 0)
            {
                while(indexes.back() < currentBlock->nodes.size())
                {
                    Node* node = currentBlock->nodes[indexes.back()];
                    
                    if(node->kind == Node::Block)
                    {
                        indexes.push_back(0);
                        currentBlock = static_cast<Block*>(node);
                    }
                    else if(node->kind == Node::Chunk)
                    {
                        interpret(static_cast<Chunk*>(node),script_);
                        ++indexes.back();
                    }
                    else
                    {
                        std::cout<<"Interpreter Error\n";
                        return;
                    }
                }
                
                currentBlock = static_cast<Block*>(currentBlock->parent);
                indexes.pop_back();
                if(indexes.size() > 0)
                    ++indexes.back();
            }
        }
        
        /// <summary>
        /// Creates and registers an InterpreterSession and run the lua script passed in argument
        /// </summary>
        /// Without InterpreterSession properly registered to ces::IO, the calls
        /// "CES.out" from Lua can not be redirected to the interpreter output stream (out_)
        /// The script passed should be the one create via Interpreter::createScript.
        /// However CES supports custom script.
        template<typename _OutputStream>
        void runScript(unsigned sesssionID_, LuaVM* lvm_, ScriptStream& script_, _OutputStream& out_)
        {
            /// Create new Streamed session
            InterpreterSession* session = new StreamedInterpreterSession<_OutputStream>(out_);
            session->id = sesssionID_;
            
            /// Register to CES I/O
            IO::registerSession(session);
            
            lvm_->runChunk(script_.str());
            
            IO::unregisterSession(session);
            delete session;
        }

        /// <summary>
        /// Interpret a Chunk
        /// </summary>
        /// This methods create a temporary lua script
        virtual void interpret(Chunk* c_, ScriptStream& script_ )
        {
            switch (c_->type)
            {
                case Node::Embedded:
                    script_<<c_->content;
                    break;
                case Node::Source:
                    script_<<" CES.out(sessionID,\""+c_->content+"\")\n";
                    break;
                case Node::Out:
                    script_<<" CES.out(sessionID,"+c_->content+")\n";
                    break;
                default:
                    break;
            }
        }
    };
}


#endif
