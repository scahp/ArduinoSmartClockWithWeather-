using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;
using System.Drawing;
using System.Drawing.Imaging;

namespace BmpToByteArray
{
    class Program
    {
		// Save Image Data to string
		// Image Data를 string을 저장
        static bool SaveDataToString(ref string result, byte[] Data, string fileName, string prefix = "IMG_")
        {
            try
            {
                result += "const unsigned char PROGMEM " + prefix + System.IO.Path.GetFileNameWithoutExtension(fileName) + "[] = {\n";

                int col = 10;
                int count = 0;
                foreach (var a in Data)
                {
                    result += string.Format("0x{0:x2}, ", a);

                    if (0 == ((++count) % col))
                    {
                        result += "\n";
                    }
                }
                result += "\n};\n";
            }
            catch
            {
                //...
                return false;
            }

            return true;
        }

		// Save string to file
		// String을 파일로 저장
        static bool SaveStringToFile(string fileName, string data)
        {
            System.IO.TextWriter Writer = null;
            try
            {
                Writer = new System.IO.StreamWriter(fileName);
                Writer.WriteLine(data);
                Writer.Flush();
                Writer.Close();
            }
            catch
            {
                //...
                return false;
            }

            return true;
        }

		// Save Data to file
		// Data를 File에 저장
        static bool SaveData(string FileName, byte[] Data)
        {
            System.IO.TextWriter Writer = null;

            try
            {
                Writer = new System.IO.StreamWriter(FileName);

                string name = "const unsigned char PROGMEM " + System.IO.Path.GetFileNameWithoutExtension(FileName) + "[] = {";
                Writer.WriteLine(name);

                int col = 10;
                int count = 0;
                foreach (var a in Data)
                {
                    Writer.Write(string.Format("0x{0:x2}, ", a));

                    if (0 == ((++count) % col))
                    {
                        Writer.WriteLine();
                    }
                }
                Writer.WriteLine("};");

                Writer.Flush();
                Writer.Close();
            }
            catch
            {
                //...
                return false;
            }

            return true;
        }

		// Image to Byte
		// 이미지를 바이트로 변환
        public static byte[] ImageToByte(Image img)
        {
            ImageConverter converter = new ImageConverter();
            return (byte[])converter.ConvertTo(img, typeof(byte[]));
        }

		// Bitmap to Byte
		// 비트맵을 바이트로 변환
        public static byte[] BitmapToByteArray(string sourceImageFilePath)
        {
            Bitmap bitmap = new Bitmap(sourceImageFilePath);
            if (null == bitmap)
            {
                return null;
            }

            BitmapData bmpdata = null;

            try
            {
                bmpdata = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, bitmap.PixelFormat);

                int numbytes = bmpdata.Stride * bitmap.Height;
                byte[] bytedata = new byte[numbytes];
                IntPtr ptr = bmpdata.Scan0;

                System.Runtime.InteropServices.Marshal.Copy(ptr, bytedata, 0, numbytes);

                // 255 -> 7, because the dot matrix display using 0~7 for R, G, B respectively.
				// 255 -> 7, 도트매트릭스의 RGB 각각 색상 값은 0~7 값을 가집니다.
                byte[] bytedata24Bit = new byte[(numbytes / 4) * 2];
                int cnt = 0;
                for (int i = 0; i < numbytes; i+=4)
                {
                    bytedata24Bit[cnt] = (byte)(((int)((bytedata[i + 0] / 255.0f) * 7.0f)) << 3);
                    bytedata24Bit[cnt] |= (byte)((bytedata[i + 1] / 255.0f) * 7.0f);
                    ++cnt;

                    bytedata24Bit[cnt] = (byte)(((int)((bytedata[i + 2] / 255.0f) * 7.0f)) << 3);
                    bytedata24Bit[cnt] |= (byte)((bytedata[i + 3] / 255.0f) * 7.0f);
                    ++cnt;
                }

                return bytedata24Bit;
            }
            finally
            {
                if (bmpdata != null)
                    bitmap.UnlockBits(bmpdata);
            }

            return null;
        }

        //static void GenerateByteArray(string targetFilePath, string sourceImageFilePath)
        //{
        //    var bitmapArray = BitmapToByteArray(sourceImageFilePath);
        //    if (null != bitmapArray)
        //    {
        //        SaveData(targetFilePath, bitmapArray);
        //    }
        //}

        // Convert bitmap to Arduino data file
        static void GenerateByteArrayFromDirectory(string targetFilePath, string sourceDirectory)
        {
            var args = System.IO.Directory.GetFiles(sourceDirectory);
            if (0 < args.Length)
            {
                string stringResult = "";

                foreach (string path in args)
                {
                    if (!System.IO.File.Exists(path))
                    {
                        continue;
                    }

                    var bitmapArray = BitmapToByteArray(path);
                    if (null != bitmapArray)
                    {
                        SaveDataToString(ref stringResult, bitmapArray, path);
                    }
                }

                SaveStringToFile(targetFilePath, stringResult);
            }
        }

        static void Main(string[] args)
        {
			// example
            //string srcPath = "C:/Users/scahp/Downloads/weather_icons/20/";
            //string destPath = "C:/Users/scahp/Downloads/weather_icons/20/conv/";
            //for (int i=0;i<42;++i)
            //{
            //    string srcFileName = string.Format("a{0:D2}.png", i + 1);
            //    string destFileName = string.Format("a{0:D2}.h", i + 1);
            //    GenerateByteArray(destPath + destFileName, srcPath + srcFileName);
            //}

            //GenerateByteArrayFromDirectory("C:/Users/scahp/Downloads/weather_icons/20/img_weather.h", srcPath);
			
            // args[0] : Result File Path
            // args[1] : Source Directory Path
			GenerateByteArrayFromDirectory(args[0], args[1]);
        }
    }
}
