using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server.Extension
{
    public enum InterChatCommandType
    {
        InformLoggedUser,
        InformLoggedUsers,
        InformLogoutUser,

        CheckUserName,

        ChangeColor,
        Speech,
    }
}
