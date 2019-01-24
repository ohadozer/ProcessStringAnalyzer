using System;
using System.Text;
using TcpLib;

namespace SimpleServer
{
    class MyServiceProvider : TcpServiceProvider
    {
        private FileRecordHandler _recordHandler = null;

        public override object Clone()
        {
            return new MyServiceProvider();
        }

        public override void OnAcceptConnection(ConnectionState state)
        {
            Console.WriteLine("New connection from " + state.RemoteEndPoint.ToString());
            _recordHandler = new FileRecordHandler(state);
        }

        public override void OnReceiveData(ConnectionState state)
        {
            if (_recordHandler != null)
            {
                int data_size = state.AvailableData;
                byte[] data = new byte[data_size];

                int bytesRead = state.Read(data, 0, data_size);

                //use a file handler to process the new data
                if (bytesRead > 0)
                {
                    _recordHandler.Write(data, bytesRead);
                }
            }
        }

        public override void OnDropConnection(ConnectionState state)
        {
            Console.WriteLine("Session ended for " + state.RemoteEndPoint.ToString());

            if (_recordHandler != null)
            {
                _recordHandler.Destroy();
            }
        }
    }
}
