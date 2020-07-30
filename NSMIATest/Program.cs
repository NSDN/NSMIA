using System;
using NSMIALib.HID;

namespace NSMIATest
{
    class Program
    {
        static void Main(string[] args)
        {
            Hid hid = new Hid();
            IntPtr ptr = new IntPtr();
            hid.DataReceived += Hid_DataReceived;
            if ((int)(ptr = hid.OpenDevice(0x3232, 0x0001, 0x02)) != -1)
            {
                Console.WriteLine("sending...");
                Report report = new Report(0x55, new byte[]{ 0x00, 0x55, 0xAA, 0xFF });
                hid.Write(report);
            }
            Console.ReadKey(true);
        }

        private static void Hid_DataReceived(object sender, Report e)
        {
            Console.Write("[" + e.reportID.ToString("X2") + "] ");
            foreach (byte b in e.reportBuff)
                Console.Write(b.ToString("X2") + " ");
            Console.WriteLine();
        }

    }
}
