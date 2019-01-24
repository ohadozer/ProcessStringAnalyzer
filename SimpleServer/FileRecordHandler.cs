using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TcpLib;
using System.Net;
using System.IO;
using System.Reflection;

namespace SimpleServer
{
    public class FileRecordHandler
    {
        private static byte[] _newlineOnlyBuf = Encoding.ASCII.GetBytes(Environment.NewLine);
        private FileStream _filestream = null;

        public FileRecordHandler(ConnectionState state)
        {
            IPEndPoint remoteIpEndPoint = state.RemoteEndPoint as IPEndPoint;
            string stFileName = string.Format("{0}___{1}.txt", remoteIpEndPoint.Address.ToString(), DateTime.Now.ToString("dd-MM-yyyy_HH-mm-ss"));

            try
            {
                string stDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                string stFile = Path.Combine(stDir, stFileName);

                _filestream = File.Create(stFile);
            }
            catch (Exception)
            {
                Console.WriteLine("Error occured, FileRecordHandler couldn't save file");
            }
        }

        public bool Write(byte[] data, int data_len)
        {
            bool bResult = false;

            if (_filestream != null)
            {
                string stData = Encoding.UTF8.GetString(data, 0, data_len);
                Console.WriteLine(stData);

                try
                {
                    _filestream.Write(data, 0, data_len);
                    //_filestream.Write(_newlineOnlyBuf, 0, _newlineOnlyBuf.Length);

                    bResult = true;
                }
                catch (Exception)
                {
                    Console.WriteLine("Error on writing to file");
                }
            }


            return bResult;
        }

        public void Destroy()
        {
            if (_filestream != null)
            {
                _filestream.Flush();
                _filestream.Close();
            }
        }
    }
}
