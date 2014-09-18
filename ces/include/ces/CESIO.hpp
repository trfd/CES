///
///  CESIO.hpp
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

#ifndef CES_CESIO_hpp
#define CES_CESIO_hpp

#include <map>

#include "Singleton.hpp"

#include "InterpreterSession.hpp"

namespace ces
{
    /// <summary>
    /// I/O interface for Lua / CES
    /// </summary>
    /// This class register interpreter session
    /// and forward CES.out call from lua to
    /// corresponding interpreter sessions
    class IO : public ck::utils::Singleton<IO>
    {
    public:
        
        typedef InterpreterSession              Session;
        typedef std::map<unsigned,Session*>     SessionMap;
        typedef SessionMap::iterator            SessionMap_it;
        
        inline static IO* instance(){return ck::utils::Singleton<IO>::instance();}
        
        /// <summary>
        /// Registers an interpreter sessions
        /// </summary>
        /// When IO receive an out call with the session's id
        /// IO forward output to the session.
        static void registerSession(Session* s_)
        {
            instance()->m_sessions[s_->id] = s_;
        }
        
        /// <summary>
        /// Unregisters an interpreter sessions
        /// </summary>
        static void unregisterSession(Session* s_)
        {
            instance()->m_sessions.erase(s_->id);
        }
        
        /// <summary>
        /// Forward a CES.out call from lua to interpreter session that matches id.
        /// </summary>
        inline static void out(unsigned sessionID_,const char* str_)
        {
            SessionMap_it it = instance()->m_sessions.find(sessionID_);
            
            if(it != instance()->m_sessions.end())
                it->second->out(str_);
        }
        
    private:
        
        
        /// <summary>
        /// Map of all registered session by ID.
        /// </summary>
        SessionMap m_sessions;
    };
}

#endif
