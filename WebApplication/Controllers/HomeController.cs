using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace SmartBubbleMachine.Controllers {


    public class HomeController : Controller {
        public ActionResult Index() {
            return View();
        }

        public ActionResult About() {
            ViewBag.Message = "Your application description page.";
            return View();
        }

        public ActionResult Contact() {
            ViewBag.Message = "Your contact page.";
            return View();
        }

        public ActionResult GetApp() {
            String FilePath = "/Content/New/SmartBubbleMachineRemote.apk"; //file path on server
            String FileName = "SmartBubbleMachineRemote.apk"; //file name
            byte[] DataBuffer = new byte[1024]; //buffer
            int DataRead;
            System.IO.Stream InputStream = new System.IO.FileStream(Server.MapPath(FilePath),
            System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
            /** Creating HTTP header **/
            HttpContext.Response.ContentType = "application/octet";

            HttpContext.Response.AddHeader("Content-Disposition",
            "attachment;filename=" + FileName);

            HttpContext.Response.BufferOutput = false;

            System.IO.Stream OutputStream = HttpContext.Response.OutputStream;
            try {
                while ((DataRead = InputStream.Read(DataBuffer, 0, DataBuffer.Length)) > 0) {
                    OutputStream.Write(DataBuffer, 0, DataRead);
                }
            } catch (Exception e) {

            } 
            
            return View("Index");
        }

        public ActionResult GetSimulator() {
            String FilePath = "/Content/WiflyModuleSimulator.zip"; //file path on server
            String FileName = "WiflyModuleSimulator.zip"; //file name
            byte[] DataBuffer = new byte[1024]; //buffer
            int DataRead;
            System.IO.Stream InputStream = new System.IO.FileStream(Server.MapPath(FilePath),
            System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
            /** Creating HTTP header **/
            HttpContext.Response.ContentType = "application/octet";

            HttpContext.Response.AddHeader("Content-Disposition",
            "attachment;filename=" + FileName);

            HttpContext.Response.BufferOutput = false;

            System.IO.Stream OutputStream = HttpContext.Response.OutputStream;
            try {
                while ((DataRead = InputStream.Read(DataBuffer, 0, DataBuffer.Length)) > 0) {
                    OutputStream.Write(DataBuffer, 0, DataRead);
                }
            } catch (Exception e) {

            }

            return View("About");
        }

        public ActionResult DownloadFirmware() {
            Download("/Content/SBM_mini.zip", "SBM_mini.zip");
            return View("Index");
        }

        public ActionResult DownloadLinearna() {
            Download("/Content/HW07-0036470555.zip", "HW07-0036470555.zip");
            return View("Index");
        }

        public ActionResult DownloadBreakout() {
            Download("/Content/TLV320AIC3104_breakout.zip", "TLV320AIC3104_breakout.zip");
            return View("Index");
        }
        public void Download (string filePath, string fileName) {
            byte[] DataBuffer = new byte[1024]; //buffer
            int DataRead;
            System.IO.Stream InputStream = new System.IO.FileStream(Server.MapPath(filePath),
            System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
            /** Creating HTTP header **/
            HttpContext.Response.ContentType = "application/octet";

            HttpContext.Response.AddHeader("Content-Disposition",
            "attachment;filename=" + fileName);

            HttpContext.Response.BufferOutput = false;

            System.IO.Stream OutputStream = HttpContext.Response.OutputStream;
            try {
                while ((DataRead = InputStream.Read(DataBuffer, 0, DataBuffer.Length)) > 0) {
                    OutputStream.Write(DataBuffer, 0, DataRead);
                }
            } catch (Exception e) {

            }
        }
    }
}