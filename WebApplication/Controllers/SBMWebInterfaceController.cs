using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using SmartBubbleMachine.Models;
using SmartBubbleMachine.Hubs;

namespace SmartBubbleMachine.Controllers
{
    public class SBMWebInterfaceController : Controller
    {
        // GET: SBMWebInterface

        private static string terminalText = null;

        private static Queue<string> terminalQueue = new Queue<string>();

        private static Dictionary<string, List<string>> terminalData = new Dictionary<string, List<string>>();

        private static int currentFuelData = 0;
        
        [Authorize]
        public ActionResult Index()
        {
            ViewBag.motorSpeed = MachineData.motorSpeed;
            ViewBag.motorState = MachineData.motorState;
            ViewBag.machineTime = MachineData.machineTime;
            ViewBag.fuelLevel = MachineData.fuelLevel;
            ViewBag.enable = MachineData.enable;
            ViewBag.terminalText = "Session started by user: " + HttpContext.Request.UserHostAddress;
            return View();
        }

        public ActionResult ChangeMotorSpeed(int motorSpeed) {
            MachineData.motorSpeed = motorSpeed;
            NotificationHub.UpdateMotorSpeed(MachineData.motorSpeed);
            NotificationHub.Send("Speed changed by user: " + HttpContext.Request.UserHostAddress + " current value: " + MachineData.motorSpeed);
            return View("Index");
        }

        public ActionResult ChangeMotorState(Boolean motorState) {
            MachineData.motorState = motorState;
            NotificationHub.UpdateMotorState(MachineData.motorState);
            NotificationHub.Send("Motor state changed by user: " + HttpContext.Request.UserHostAddress + " current value: " + MachineData.motorState);
            return View("Index");
        }

        public ActionResult GetFuelLevel() {
            if (Request.IsAjaxRequest()) {
                if (currentFuelData != MachineData.fuelLevel) {
                    NotificationHub.Send("Fuel level updated on: " + MachineData.machineTime + " current value:" + MachineData.fuelLevel);
                    NotificationHub.UpdateFuelLevel(MachineData.fuelLevel);
                    currentFuelData = MachineData.fuelLevel;
                }
                return Json(MachineData.fuelLevel, JsonRequestBehavior.AllowGet);
            }
            
            return View("Index");
        }

        public ActionResult GetMotorState() {
            if (Request.IsAjaxRequest()) {
                return Json(MachineData.motorState, JsonRequestBehavior.AllowGet);
            }
            return View("Index");
        }

        public ActionResult GetMotorSpeed() {
            if (Request.IsAjaxRequest()) {
                return Json(MachineData.motorSpeed, JsonRequestBehavior.AllowGet);
            }
            return View("Index");
        }

        public ActionResult GetMachineTime() {
            if (Request.IsAjaxRequest()) {
                String data = DateTime.Now.TimeOfDay.ToString();
                return Json(MachineData.machineTime, JsonRequestBehavior.AllowGet);
            }
            return View("Index");
        }

        public ActionResult GetTerminalData() {
            if (Request.IsAjaxRequest()) {
                if (terminalText != null) {
                    string forExport = new string(terminalText.ToCharArray());
                    terminalText = null;
                    return Json(forExport, JsonRequestBehavior.AllowGet);
                } else {
                    return Json(terminalText, JsonRequestBehavior.AllowGet);
                }
            }
            return View("Index");
        }


        public ActionResult WiflyAccess(int fuelLevel, string machineTime) {
            MachineData.machineTime = machineTime;
            if (MachineData.fuelLevel != fuelLevel) {
                MachineData.fuelLevel = fuelLevel;
                NotificationHub.Send("Fuel level updated on: " + MachineData.machineTime + " current value:" + MachineData.fuelLevel);
                NotificationHub.UpdateFuelLevel(MachineData.fuelLevel);
            }
            ViewBag.request = HttpContext.Request.HttpMethod;
            NotificationHub.UpdateTime(MachineData.machineTime);
            string motorState = (MachineData.motorState) ? "1" : "0";
            ViewBag.wiflyData = "!01$" + motorState + "&" + "02" + "$" + MachineData.motorSpeed + "#";
            ViewBag.motorSpeed = MachineData.motorSpeed;
            ViewBag.motorState = MachineData.motorState ? "uključen" : "isključen";
            return View();
        }

        public ActionResult WiflyAccess2(string data) {
            string[] parameters = data.Split('i');
            string machineTime = parameters[1].Replace("t", ":");
            int fuelLevel = int.Parse(parameters[0]);
            MachineData.machineTime = machineTime;
            if (MachineData.fuelLevel != fuelLevel) {
                MachineData.fuelLevel = fuelLevel;
                NotificationHub.Send("Fuel level updated on: " + MachineData.machineTime + " current value:" + MachineData.fuelLevel);
                NotificationHub.UpdateFuelLevel(MachineData.fuelLevel);
            }
            ViewBag.request = HttpContext.Request.HttpMethod;
            NotificationHub.UpdateTime(MachineData.machineTime);
            string motorState = (MachineData.motorState) ? "1" : "0";
            if (MachineData.enable) { 
                ViewBag.wiflyData = "!01$" + motorState + "#" + "!" +  "02" + "$" + MachineData.motorSpeed + "#";
            } else {
                ViewBag.wiflyData = "";
            }
            ViewBag.motorSpeed = MachineData.motorSpeed;
            ViewBag.motorState = MachineData.motorState ? "uključen" : "isključen";
            return View();
        }



        
    }
}