using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DWServer
{
    public class DWDML
    {
        public static LookupService _geoIP = new LookupService("GeoLiteCity.dat", LookupService.GEOIP_STANDARD);

        public static void DW_PacketReceived(MessageData data)
        {
            var type = data.Get<int>("type");
            var crypt = data.Get<bool>("crypt");

            var packet = DWRouter.GetMessage(data);
            var call = packet.ByteBuffer.ReadByte();

            switch (call)
            {
                case 2:
                    GetUserData(data, packet);
                    break;
            }
        }

        private static void GetUserData(MessageData mdata, DWMessage packet)
        {
            var ip = mdata.Get<string>("cid").Split(':')[0];
            var location = _geoIP.getLocation(ip);

            var reply = packet.MakeReply(1, false);
            reply.ByteBuffer.Write(0x8000000000000001);
            reply.ByteBuffer.Write((uint)0);
            reply.ByteBuffer.Write((byte)8);
            reply.ByteBuffer.Write(1);
            reply.ByteBuffer.Write(1);

            reply.ByteBuffer.Write(location.countryCode ?? "");
            reply.ByteBuffer.Write(location.countryName ?? "");
            reply.ByteBuffer.Write(location.regionName ?? "");
            reply.ByteBuffer.Write(location.city ?? "");
            reply.ByteBuffer.Write((float)location.latitude);
            reply.ByteBuffer.Write((float)location.longitude);
            Log.Info(string.Format("Sending reply to GetUserData, packet 2, data: {0} - {1} - {}.", location.countryCode, location.countryName, location.regionName));

            reply.Send(true);
        }
    }
}
