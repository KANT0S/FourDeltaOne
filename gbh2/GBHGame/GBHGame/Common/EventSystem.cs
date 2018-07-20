using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework.Input;

namespace GBH
{
    public static class EventSystem
    {
        private static Queue<Event> _eventQueue = new Queue<Event>();

        // called from the main loop
        public static uint HandleEvents()
        {
            while (true)
            {
                Event ev = GetEvent();

                if (ev is NullEvent)
                {
                    HandleLoopbackPackets();

                    return ev.Time;
                }

                ev.Handle();
            }
        }

        private static void HandleLoopbackPackets()
        {
            byte[] packet;

            while ((packet = NetManager.GetLoopbackPacket(NetChannelType.ClientToServer)) != null)
            {
                Server.ProcessPacket(NetAddress.Loopback, packet);
            }

            while ((packet = NetManager.GetLoopbackPacket(NetChannelType.ServerToClient)) != null)
            {
                Client.ProcessPacket(NetAddress.Loopback, packet);
            }
        }

        public static void QueueEvent(Event ev)
        {
            _eventQueue.Enqueue(ev);
        }

        private static Event GetEvent()
        {
            GameWindow.RunEvents();

            if (_eventQueue.Count > 0)
            {
                return _eventQueue.Dequeue();
            }

            if (_timeEvent == null)
            {
                _timeEvent = new NullEvent();
            }

            _timeEvent.Time = Game.Time;

            return _timeEvent;
       }

        private static NullEvent _timeEvent;
    }

    public abstract class Event
    {
        public uint Time { get; set; }

        public abstract void Handle();
    }

    class NullEvent : Event
    {
        public override void Handle()
        {
            // do nothing, this event isn't meant to be handled
        }
    }

    class KeyEvent : Event
    {
        public Keys Key { get; set; }
        public bool Down { get; set; }

        public override void Handle()
        {
            if (Key == Keys.F11 && Down)
            {
                ConsoleRenderer.Active = !ConsoleRenderer.Active;
                return;
            }

            if (ConsoleRenderer.Active && Down)
            {
                ConsoleRenderer.HandleKey(Key);
                return;
            }

            //DebugCamera.HandleKey(Key, Down);
            ClientInput.HandleKey(Key, Down);
        }
    }

    class CharEvent : Event
    {
        public char Character { get; set; }

        public override void Handle()
        {
            if (ConsoleRenderer.Active)
            {
                ConsoleRenderer.HandleChar(Character);
                return;
            }
        }
    }
}
