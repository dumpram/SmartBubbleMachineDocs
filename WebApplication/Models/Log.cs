using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ComponentModel.DataAnnotations;

namespace SmartBubbleMachine.Models {
    public class Log {

        public int ID { get; set; }
        
        [DataType(DataType.MultilineText)]
        public string data { get; set; }
    }
}