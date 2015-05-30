using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace SmartBubbleMachine.Models {
    public class MachineData {
        public static int motorSpeed { get; set; }
        public static bool motorState { get; set; }
        public static string machineTime { get; set; }
        public static int fuelLevel { get; set; }
        public static bool enable { get; set; }
    }
}