using System;
using SimpleServer;
using TcpLib;

class Program
{
    static void Main(string[] args)
    {
        if (args.Length == 2)
        {
            string stIp = args[0];
            int iPort = int.Parse(args[1]);

            TcpServer server = new TcpServer(new MyServiceProvider(), stIp, iPort);

            if (server.Start())
            {
                Console.WriteLine("listening on " + stIp + ":" + iPort);
                Console.WriteLine("Press enter any key to stop server");
                Console.ReadLine();

                server.Stop();
            }
            else
            {
                Console.Write("Error on server startup, terminating..");
            }
        }
        else
        {
            Console.Write("Usage: SimpleServer <IP Address> <Port>");
        }
    }
}