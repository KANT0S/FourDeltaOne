using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace NPx
{
    public class ScheduledTask
    {
        public Action Action { get; set; }
        public DateTime RunAt { get; set; }

        public void Run()
        {
            if (Action != null)
            {
                try
                {
                    Action();
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }
            }
        }

        public bool CanRun
        {
            get
            {
                return (DateTime.UtcNow >= RunAt);
            }
        }
    }

    public class TaskScheduler
    {
        private static Thread _thread;
        private static List<ScheduledTask> _tasks;

        public static void Start()
        {
            _tasks = new List<ScheduledTask>();

            _thread = new Thread(new ThreadStart(Run));
            _thread.Start();
        }

        private static void Run()
        {
            while (true)
            {
                Thread.Sleep(1000);

                try
                {
                    var tasks = GetCurrentTasks().ToList();
                    var toClear = new List<ScheduledTask>();

                    foreach (var task in tasks)
                    {
                        task.Run();
                        toClear.Add(task);
                    }

                    foreach (var task in toClear)
                    {
                        DequeueTask(task);
                    }
                }
                catch (Exception e)
                {
                    Log.Error(e.ToString());
                }
            }
        }

        public static void EnqueueTask(Action action, int seconds)
        {
            lock (_tasks)
            {
                _tasks.Add(new ScheduledTask() { Action = action, RunAt = (DateTime.UtcNow + TimeSpan.FromSeconds(seconds)) });
            }
        }

        private static void DequeueTask(ScheduledTask task)
        {
            lock (_tasks)
            {
                _tasks.Remove(task);
            }
        }

        private static IEnumerable<ScheduledTask> GetCurrentTasks()
        {
            return from task in _tasks
                   where task.CanRun
                   select task;
        }
    }
}
