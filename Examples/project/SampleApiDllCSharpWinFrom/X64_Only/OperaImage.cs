using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;


namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {

        private static Bitmap BuiltGrayBitmap(float[] rawValues, int width, int height)
        {
            var l = new List<float>(rawValues);
            l.RemoveAll(data => ( float.IsNaN(data)));

            var vmax = l.Max<float>();
            var vmin = l.Min<float>();
            var vRange = vmax - vmin;

            if (0 == vRange)
                vRange = 1.0f; // Avoid error "devide by zero".

            byte[] arrayByte = new byte[rawValues.Length];
            for (var i = 0; i < rawValues.Length; ++i)
               if(!float.IsNaN(rawValues[i]))
                    arrayByte[i] = Convert.ToByte((rawValues[i] - vmin) / vRange * 255);

            return BuiltGrayBitmap(arrayByte, width, height);
        }

        private static Bitmap BuiltGrayBitmap(ushort[] rawValues, int width, int height)
        {
            var l = new List<ushort>(rawValues);
            var vmax = l.Max();
            var vmin = l.Min();
            var vRange = vmax - vmin;

            if (0 == vRange)
                vRange = 1; // Avoid error "devide by zero".

            byte[] arrayByte = new byte[rawValues.Length];
            for (var i=0; i< rawValues.Length; ++i)
                arrayByte[i] = Convert.ToByte((double)(rawValues[i] - vmin) / vRange * 255);

            return BuiltGrayBitmap(arrayByte, width, height);
        }

        // from: https://www.cnblogs.com/lonelyxmas/p/7727303.html
        private static Bitmap BuiltGrayBitmap(byte[] rawValues, int width, int height)
        {
            Bitmap bitmap = new Bitmap(width, height, PixelFormat.Format8bppIndexed);
            BitmapData bmpData = bitmap.LockBits(new Rectangle(0, 0, width, height),
                 ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);

            int offset = bmpData.Stride - bmpData.Width;        
            IntPtr ptr = bmpData.Scan0;                         
            int scanBytes = bmpData.Stride * bmpData.Height;    

            Marshal.Copy(rawValues, 0, ptr, scanBytes);
            bitmap.UnlockBits(bmpData); 

            ColorPalette palette;
            using (Bitmap bmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
            {
                palette = bmp.Palette;
            }

            for (int i = 0; i < 256; ++i)
            {
                palette.Entries[i] = Color.FromArgb(i, i, i);
            }
            bitmap.Palette = palette;

            return bitmap;
        }


    }

}


