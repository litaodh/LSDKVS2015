using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using SICK3D;

namespace SampleApiDllCSharpConsole
{
    class Program
    {
        static void unit_test_print(bool isPrintToScream, string log, Object obj)
        {
            if (isPrintToScream)
                Console.WriteLine(log, obj);
        }
        static void unit_test_print(bool isPrintToScream, string log)
        {
            unit_test_print(isPrintToScream, log, "");
        }


        /////////////////////////////////////////////////////////////////////////////////////


        static bool test_CS_basic_functions(bool isPrintToScream = true)
        {
            bool enable_CPP_Grab_thread = false;

            // --- test CSR3S. (Please note: "CS" = C_Sharp, "R3S" = Ranger3_Shared_runtime_environment, So "CSR3S" = class of C_Sharp_Ranger3_Shared_runtime_environment.)
            String logPath = "D:\\log.txt";
            String ctiPath = "";
            bool isPrintLog = isPrintToScream;
            bool isWriteLog = true;
            GenICam3D r3s = new GenICam3D(logPath, ctiPath, isPrintLog, isWriteLog);

            String[] IPs = r3s.getConDevListIP();
            String[] Nas = r3s.getConDevList();
            var scanRes = r3s.scanDevice();

            // --- test CSR3. (Please note: "CS" = C_Sharp, "R3" = Ranger3, So "CSR3" = class of C_Sharp_Ranger3.)
            String mac = "0006770c69e1";
            Ranger3 r3 = new Ranger3(mac, false);
            /// CSR3 r3 = new CSR3(IPs[0], true);

            if (r3.isReady() != CAM_STATUS.All_OK)
            {
                if(isPrintToScream)  unit_test_print(isPrintToScream, "Camera is not ready!");
                return false;
            }

            var camStatus = r3.connectCamera(enable_CPP_Grab_thread);
            if (camStatus != CAM_STATUS.All_OK)
            {
                if (isPrintToScream)  unit_test_print(isPrintToScream, "Camera connecting failed!");
                return false;
            }

            /// test CS getting paramters info
            if (r3.updateParamtersInfo() == false)
                return false;

            String[] CategoryList = r3.paramCategoryList();
            String[] FullNameList = r3.paramFullNameList();
            int fnId = 0;

            unit_test_print(isPrintToScream, "");
            unit_test_print(isPrintToScream, "--------------");
            foreach (String cate in CategoryList)
            {
                unit_test_print(isPrintToScream, "|");
                unit_test_print(isPrintToScream, "----  Cate: {0}", cate);
                String[] categries = r3.paramFullNameOfCategory(cate);
                foreach (String fn in categries)
                {
                    unit_test_print(isPrintToScream, "    |");  unit_test_print(isPrintToScream, "    --- FullName: {0}", fn);
                    unit_test_print(isPrintToScream, "        |");  unit_test_print(isPrintToScream, "        --- CurrentValue: {0}", r3.paramCurrentValue(fn));
                    unit_test_print(isPrintToScream, "        |");  unit_test_print(isPrintToScream, "        --- Optional values:");
                    String[] opt = r3.paramOptionalValues(fn, ref fnId);
                    foreach (String sub in opt)
                    {
                         unit_test_print(isPrintToScream, "               |");
                         unit_test_print(isPrintToScream, "               ---- {0}", sub);
                    }
                }
            }


            // --- test CSR3 setting paramters info
            ///r3.setParameterValue("DeviceScanType", "Linescan3D");    /// 3D Range image
            r3.setParameterValue("DeviceScanType", "Areascan");         /// 2D Sensor image



            camStatus = r3.startCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera starting failed!");
                return false;
            }

            // --- test CSR3 acquisition
            Thread.Sleep(1000);

            // --- test CS ImgT
            CSImgT imgT = new CSImgT();
            camStatus = r3.getImageData(imgT);

            for (DataNames i = 0; i < DataNames.SCA2_CAL; ++i)
            {
                 unit_test_print(isPrintToScream, imgT.DN2Str(i));
            }
            // --- test CSR3 ImgT data array
            byte[] senData = imgT.getDataByte(DataNames.SEN);

            // --- test CSImgT data info getting
            int S_cols = imgT.getS_cols();
            int S_rows = imgT.getS_rows();
            int S_OffsetX = imgT.getS_OffsetX();
            int S_OffsetY = imgT.getS_OffsetY();
            int R_cols = imgT.getR_cols();
            int R_rows = imgT.getR_rows();
            int R_AoiOffsetX = imgT.getR_AoiOffsetX();
            int R_AoiOffsetY = imgT.getR_AoiOffsetY();
            int R_AoiHeight = imgT.getR_AoiHeight();
            int R_AoiWidth = imgT.getR_AoiWidth();
            bool R_RangeAxis = imgT.getR_RangeAxis();
            int C_cols = imgT.getC_cols();
            int C_rows = imgT.getC_rows();
            double C_offsetX = imgT.getC_offsetX();
            double C_offsetY = imgT.getC_offsetY();
            double C_scaleX = imgT.getC_scaleX();
            double C_scaleY = imgT.getC_scaleY();
            double C_lower_bound_x = imgT.getC_lower_bound_x();
            double C_upper_bound_x = imgT.getC_upper_bound_x();
            double C_lower_bound_r = imgT.getC_lower_bound_r();
            double C_upper_bound_r = imgT.getC_upper_bound_r();

            /// test CS ImgT data info getting
            int id = imgT.get_ID();
            bool isE = imgT.isEmpty();
            bool hasS = imgT.has(DataNames.SEN);
            bool hasR = imgT.has(DataNames.RAN);
            bool hasCI = imgT.has_ChunkInfo();

            bool saveSenOk = imgT.SaveToIconFile("D:\\sen", DataNames.SEN);
            bool saveRanOk = imgT.SaveToIconFile("D:\\ran", DataNames.RAN);
            bool saveRacOk = imgT.SaveToIconFile("D:\\rac", DataNames.RAN_CAL);

            camStatus = r3.stopCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera stopping failed!");
                return false;
            }

            camStatus = r3.disconnectCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera disconnecting failed!");
                return false;
            }
            return true;
        }

        static void test_CS_CPP_Graber_thread_process_img(bool isPrint, CSImgT imgT)
        {
            for (DataNames i = 0; i < DataNames.SCA2_CAL; ++i)
            {
                unit_test_print(isPrint, imgT.DN2Str(i));
            }
            // --- test CSR3 ImgT data array
            byte[] senData = imgT.getDataByte(DataNames.SEN);

            // --- test CSImgT data info getting
            int cols = imgT.get_cols(DataNames.SEN);
            int rows = imgT.get_rows(DataNames.SEN);

            int S_cols = imgT.getS_cols();
            int S_rows = imgT.getS_rows();
            int S_OffsetX = imgT.getS_OffsetX();
            int S_OffsetY = imgT.getS_OffsetY();
            int R_cols = imgT.getR_cols();
            int R_rows = imgT.getR_rows();
            int R_AoiOffsetX = imgT.getR_AoiOffsetX();
            int R_AoiOffsetY = imgT.getR_AoiOffsetY();
            int R_AoiHeight = imgT.getR_AoiHeight();
            int R_AoiWidth = imgT.getR_AoiWidth();
            bool R_RangeAxis = imgT.getR_RangeAxis();
            int C_cols = imgT.getC_cols();
            int C_rows = imgT.getC_rows();
            double C_offsetX = imgT.getC_offsetX();
            double C_offsetY = imgT.getC_offsetY();
            double C_scaleX = imgT.getC_scaleX();
            double C_scaleY = imgT.getC_scaleY();
            double C_lower_bound_x = imgT.getC_lower_bound_x();
            double C_upper_bound_x = imgT.getC_upper_bound_x();
            double C_lower_bound_r = imgT.getC_lower_bound_r();
            double C_upper_bound_r = imgT.getC_upper_bound_r();

            /// test CS ImgT data info getting
            int id = imgT.get_ID();
            bool isE = imgT.isEmpty();
            bool hasS = imgT.has(DataNames.SEN);
            bool hasR = imgT.has(DataNames.RAN);
            bool hasCI = imgT.has_ChunkInfo();

            bool saveSenOk = imgT.SaveToIconFile("D:\\sen", DataNames.SEN);
            bool saveRanOk = imgT.SaveToIconFile("D:\\ran", DataNames.RAN);
            bool saveRacOk = imgT.SaveToIconFile("D:\\rac", DataNames.RAN_CAL);

            unit_test_print(true, "Get Image, id = {0}", id);
        }
        static bool test_CS_CPP_Graber_thread(bool isPrintToScream = true, int nImg=5)
        {
            bool enable_CPP_Grab_thread = true;

            // --- test CSR3S. (Please note: "CS" = C_Sharp, "R3S" = Ranger3_Shared_runtime_environment, So "CSR3S" = class of C_Sharp_Ranger3_Shared_runtime_environment.)
            String logPath = "D:\\log.txt";
            String ctiPath = "";
            bool isPrintLog = isPrintToScream;
            bool isWriteLog = true;
            GenICam3D r3s = new GenICam3D(logPath, ctiPath, isPrintLog, isWriteLog);

            String[] IPs = r3s.getConDevListIP();
            String[] Nas = r3s.getConDevList();
            var scanRes = r3s.scanDevice();

            // --- test CSR3. (Please note: "CS" = C_Sharp, "R3" = Ranger3, So "CSR3" = class of C_Sharp_Ranger3.)
            ///String mac = "0006770c69e1";
            ///Ranger3 r3 = new Ranger3(mac, false);
            Ranger3 r3 = new Ranger3(IPs[0], true);

            if (r3.isReady() != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera is not ready!");
                return false;
            }

            /// If program might last time, camera status will be incorrect. re-conencting to make it ready!
            ///Debug tips.
            r3.connectCamera(false);
            r3.disconnectCamera();

            var camStatus = r3.connectCamera(enable_CPP_Grab_thread);
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera connecting failed!");
                return false;
            }

            /// test CS getting paramters info
            if (r3.updateParamtersInfo() == false)
                return false;

            String[] CategoryList = r3.paramCategoryList();
            String[] FullNameList = r3.paramFullNameList();
            int fnId = 0;
            unit_test_print(isPrintToScream, "");
            unit_test_print(isPrintToScream, "--------------");
            foreach (String cate in CategoryList)
            {
                 unit_test_print(isPrintToScream, "|");
                 unit_test_print(isPrintToScream, "----  Cate: {0}", cate);
                String[] categries = r3.paramFullNameOfCategory(cate);
                foreach (String fn in categries)
                {
                     unit_test_print(isPrintToScream, "    |");  unit_test_print(isPrintToScream, "    --- FullName: {0}", fn);
                     unit_test_print(isPrintToScream, "        |");  unit_test_print(isPrintToScream, "        --- CurrentValue: {0}", r3.paramCurrentValue(fn));
                     unit_test_print(isPrintToScream, "        |");  unit_test_print(isPrintToScream, "        --- Optional values:");
                    String[] opt = r3.paramOptionalValues(fn, ref fnId);
                    foreach (String sub in opt)
                    {
                         unit_test_print(isPrintToScream, "               |");
                         unit_test_print(isPrintToScream, "               ---- {0}", sub);
                    }
                }
            }


            // --- test CSR3 setting paramters info
            ///r3.setParameterValue("DeviceScanType", "Linescan3D");    /// 3D Range image
            r3.setParameterValue("DeviceScanType", "Areascan");         /// 2D Sensor image

            // --- Memory using check: X Mb before starting
            camStatus = r3.startCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera starting failed!");
                return false;
            }

            // --- test CSR3 acquisition
            Thread.Sleep(1000);

            int i = 0;
#if true
            while (true)
            {

                if (r3.isGrabbed())
                {
                    // --- test CS ImgT
                    CSImgT imgT = new CSImgT();
                    camStatus = r3.getImageData(imgT);
                    if (camStatus != CAM_STATUS.All_OK)
                    {
                        unit_test_print(isPrintToScream, "Camera get data failed!");
                        return false;
                    }

                    test_CS_CPP_Graber_thread_process_img(isPrintToScream, imgT);

                    // --- Important to list these code here and collect memory allocating to imgT.
                    imgT.Dispose();
                    GC.Collect();
                    GC.WaitForFullGCComplete();
                    ++i;
                }
                else
                    Thread.Sleep(50);

                if (i == nImg)
                    break;
            }
#else
            while (true)
            {
                Thread.Sleep(50);
                ++i;
                Console.WriteLine("i = {0}", i);
                if (i == nImg)
                    break;
            }
#endif

            // --- Memory using check: around X Mb after stopping.
            camStatus = r3.stopCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera stopping failed!");
                return false;
            }

            camStatus = r3.disconnectCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera disconnecting failed!");
                return false;
            }

            // --- delete variable.
            r3s.Dispose();


            return true;
        }

        /// <summary>
        /// 用灰度数组新建一个8位灰度图像。
        /// </summary>
        /// <param name="rawValues"> 灰度数组(length = width * height)。 </param>
        /// <param name="width"> 图像宽度。 </param>
        /// <param name="height"> 图像高度。 </param>
        /// <returns> 新建的8位灰度位图。 </returns>
        private static Bitmap BuiltGrayBitmap(byte[] rawValues, int width, int height)
        {
            // 新建一个8位灰度位图，并锁定内存区域操作
            Bitmap bitmap = new Bitmap(width, height, PixelFormat.Format8bppIndexed);
            BitmapData bmpData = bitmap.LockBits(new Rectangle(0, 0, width, height),
                 ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);

            // 计算图像参数
            int offset = bmpData.Stride - bmpData.Width;        // 计算每行未用空间字节数
            IntPtr ptr = bmpData.Scan0;                         // 获取首地址
            int scanBytes = bmpData.Stride * bmpData.Height;    // 图像字节数 = 扫描字节数 * 高度
            byte[] grayValues = new byte[scanBytes];            // 为图像数据分配内存

            // 为图像数据赋值
            int posSrc = 0, posScan = 0;                        // rawValues和grayValues的索引
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    grayValues[posScan++] = rawValues[posSrc++];
                }
                // 跳过图像数据每行未用空间的字节，length = stride - width * bytePerPixel
                posScan += offset;
            }

            // 内存解锁
            Marshal.Copy(grayValues, 0, ptr, scanBytes);
            bitmap.UnlockBits(bmpData);  // 解锁内存区域

            // 修改生成位图的索引表，从伪彩修改为灰度
            ColorPalette palette;
            // 获取一个Format8bppIndexed格式图像的Palette对象
            using (Bitmap bmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
            {
                palette = bmp.Palette;
            }
            for (int i = 0; i < 256; i++)
            {
                palette.Entries[i] = Color.FromArgb(i, i, i);
            }
            // 修改生成位图的索引表
            bitmap.Palette = palette;

            return bitmap;
        }
        static bool test_CS_CPP_ShowImage(bool isPrintToScream = true, int nImg = 5)
        {
            bool enable_CPP_Grab_thread = true;

            // --- test CSR3S. (Please note: "CS" = C_Sharp, "R3S" = Ranger3_Shared_runtime_environment, So "CSR3S" = class of C_Sharp_Ranger3_Shared_runtime_environment.)
            String logPath = "D:\\log.txt";
            String ctiPath = "";
            bool isPrintLog = isPrintToScream;
            bool isWriteLog = true;
            GenICam3D r3s = new GenICam3D(logPath, ctiPath, isPrintLog, isWriteLog);

            String[] IPs = r3s.getConDevListIP();
            String[] Nas = r3s.getConDevList();
            var scanRes = r3s.scanDevice();

            // --- test CSR3. (Please note: "CS" = C_Sharp, "R3" = Ranger3, So "CSR3" = class of C_Sharp_Ranger3.)
            String mac = "0006770c69e1";
            Ranger3 r3 = new Ranger3(mac, false);
            /// CSR3 r3 = new CSR3(IPs[0], true);

            if (r3.isReady() != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera is not ready!");
                return false;
            }

            /// If program might last time, camera status will be incorrect. re-conencting to make it ready!
            /// Debug tips.
            r3.connectCamera(false);
            r3.disconnectCamera();

            var camStatus = r3.connectCamera(enable_CPP_Grab_thread);
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera connecting failed!");
                return false;
            }

            /// test CS getting paramters info
            if (r3.updateParamtersInfo() == false)
                return false;

            String[] CategoryList = r3.paramCategoryList();
            String[] FullNameList = r3.paramFullNameList();
            int fnId = 0;
            unit_test_print(isPrintToScream, "");
            unit_test_print(isPrintToScream, "--------------");
            foreach (String cate in CategoryList)
            {
                unit_test_print(isPrintToScream, "|");
                unit_test_print(isPrintToScream, "----  Cate: {0}", cate);
                String[] categries = r3.paramFullNameOfCategory(cate);
                foreach (String fn in categries)
                {
                    unit_test_print(isPrintToScream, "    |"); unit_test_print(isPrintToScream, "    --- FullName: {0}", fn);
                    unit_test_print(isPrintToScream, "        |"); unit_test_print(isPrintToScream, "        --- CurrentValue: {0}", r3.paramCurrentValue(fn));
                    unit_test_print(isPrintToScream, "        |"); unit_test_print(isPrintToScream, "        --- Optional values:");
                    String[] opt = r3.paramOptionalValues(fn, ref fnId);
                    foreach (String sub in opt)
                    {
                        unit_test_print(isPrintToScream, "               |");
                        unit_test_print(isPrintToScream, "               ---- {0}", sub);
                    }
                }
            }


            // --- test CSR3 setting paramters info
            ///r3.setParameterValue("DeviceScanType", "Linescan3D");    /// 3D Range image
            r3.setParameterValue("DeviceScanType", "Areascan");         /// 2D Sensor image

            camStatus = r3.startCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera starting failed!");
                return false;
            }

            // --- test CSR3 acquisition
            Thread.Sleep(1000);

            int i = 0;
            while (true)
            {

                if (r3.isGrabbed())
                {
                    // --- test CS ImgT
                    CSImgT imgT = new CSImgT();
                    camStatus = r3.getImageData(imgT);
                    if (camStatus != CAM_STATUS.All_OK)
                    {
                        unit_test_print(isPrintToScream, "Camera get data failed!");
                        return false;
                    }

                    //读取图片            
                    byte[] senImage = imgT.getDataByte(DataNames.SEN);
                    var img2 = BuiltGrayBitmap(senImage, 2560, 832);

                    //显示图片         
                    System.Drawing.Image imgXXXX = System.Drawing.Image.FromHbitmap(img2.GetHbitmap());

                    ++i;
                }
                else
                    Thread.Sleep(50);

                if (i == nImg)
                    break;
            }

            camStatus = r3.stopCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera stopping failed!");
                return false;
            }

            camStatus = r3.disconnectCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera disconnecting failed!");
                return false;
            }

            // --- delete variable.
            r3s.Dispose();


            return true;
        }


        /////////////////////////////////////////////////////////////////////////////////////


        static void Main(string[] args)
        {
            var isOK = false;

            //isOK = test_CS_basic_functions(true);
            //unit_test_print(true, "test_CS_basic_functions    {0}", isOK);

            isOK = test_CS_CPP_Graber_thread(false, 20);
            unit_test_print(true, "test_CS_CPP_Graber_thread    {0}", isOK);

            //isOK = test_CS_CPP_ShowImage(false, 9);
            //unit_test_print(true, "test_CS_CPP_ShowImage    {0}", isOK);
        }


    }
}
