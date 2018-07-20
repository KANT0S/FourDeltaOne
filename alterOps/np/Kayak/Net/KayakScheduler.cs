using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace Kayak
{
    class DefaultKayakScheduler : IScheduler
    {
        ManualResetEventSlim wh;
        ConcurrentQueue<Action> queue;
        bool stopped;
        ISchedulerDelegate del;

        internal DefaultKayakScheduler(ISchedulerDelegate del)
        {
            if (del == null)
                throw new ArgumentNullException("del");

            this.del = del;
            queue = new ConcurrentQueue<Action>();
        }

        public void Post(Action action)
        {
            NPx.Log.Debug("--- Posted task.");

            queue.Enqueue(action);
            wh.Set();
        }

        public void Start()
        {
            MainLoop();
        }

        private void MainLoop()
        {
            wh = new ManualResetEventSlim();

            while (true)
            {
                Action outTask = null;

                if (queue.TryDequeue(out outTask))
                {
                    NPx.Log.Debug("--- Executing Task ---");
                    try
                    {
                        outTask();
                    }
                    catch (Exception e)
                    {
                        NPx.Log.Error(e.ToString());
                    }
                    NPx.Log.Debug("--- Done Executing Task ---");

                    if (stopped)
                    {
                        stopped = false;
                        queue = new ConcurrentQueue<Action>();

                        NPx.Log.Debug("Scheduler stopped.");
                        del.OnStop(this);

                        break;
                    }
                }
                else
                {
                    wh.Wait();
                    wh.Reset();
                }
            }

            stopped = false;
            wh.Dispose();
            wh = null;
        }

        public void Stop()
        {
            NPx.Log.Debug("Scheduler will stop.");
            Post(() => { stopped = true; });
        }

        public void Dispose()
        {
            // nothing to see here!
        }
    }

    /*
    class DefaultKayakScheduler : TaskScheduler, IScheduler
    {
        ISchedulerDelegate del;

        Thread dispatch;
        ManualResetEventSlim wh;
        ConcurrentQueue<Task> queue;
        bool stopped;

        public void Post(Action action)
        {
            NPx.Log.Debug("--- Posted task.");

            var task = new Task(action);
            task.ContinueWith(t =>
            {
                del.OnException(this, t.Exception);
            }, CancellationToken.None, TaskContinuationOptions.OnlyOnFaulted, this);
            task.Start(this);
        }

        internal DefaultKayakScheduler(ISchedulerDelegate del)
        {
            if (del == null)
                throw new ArgumentNullException("del");

            this.del = del;
            queue = new ConcurrentQueue<Task>();
        }

        public void Start()
        {
            if (dispatch != null)
                throw new InvalidOperationException("The scheduler was already started.");

            Dispatch();
        }

        public void Stop()
        {
            NPx.Log.Debug("Scheduler will stop.");
            Post(() => { stopped = true; });
        }

        void Dispatch()
        {
            wh = new ManualResetEventSlim();

            while (true)
            {
                Task outTask = null;

                if (queue.TryDequeue(out outTask))
                {
                    NPx.Log.Debug("--- Executing Task ---");
                    TryExecuteTask(outTask);
                    NPx.Log.Debug("--- Done Executing Task ---");

                    if (stopped)
                    {
                        stopped = false;
                        dispatch = null;
                        queue = new ConcurrentQueue<Task>();

                        NPx.Log.Debug("Scheduler stopped.");
                        del.OnStop(this);

                        break;
                    }
                }
                else
                {
                    wh.Wait();
                    wh.Reset();
                }
            }

            stopped = false;
            dispatch = null;
            wh.Dispose();
            wh = null;
        }

        protected override IEnumerable<Task> GetScheduledTasks()
        {
            yield break;
        }

        public override int MaximumConcurrencyLevel { get { return 1; } }

        protected override void QueueTask(Task task)
        {
            queue.Enqueue(task);
            if (wh != null)
            {
                wh.Set();
            }
        }

        protected override bool TryDequeue(Task task)
        {
            Task outTask = null;
            queue.TryDequeue(out outTask);
            return (task == outTask);
        }

        protected override bool TryExecuteTaskInline(Task task, bool taskWasPreviouslyQueued)
        {
            if (Thread.CurrentThread != dispatch) return false;

            if (taskWasPreviouslyQueued && !TryDequeue(task))
            {
                return false;
            }

            return TryExecuteTask(task);
        }

        public void Dispose()
        {
            // nothing to see here!
        }
    }
    */
}
