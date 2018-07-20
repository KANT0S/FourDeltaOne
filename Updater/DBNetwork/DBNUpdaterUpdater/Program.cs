using System;
using System.Collections.Generic;
using System.Text;
using DBNUpdater;
using System.Diagnostics;
using System.Reflection;
using System.IO;
namespace DBNUpdater.dediclient
{
    class Program
    {

        static public void Main()
        {
            Environment.CurrentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);//Application.StartupPath;
            var updater = new Updater("http://content.aiw3.net/updater/", "", false);
            //var updater = new Updater(String.Format("{0}.content.alteriw.net/dediclient/", (char)(97 + new Random().Next(5))), "", false);
            updater.StatusChanged += updater_StatusChanged;
            updater.Start(new string[] { "iw4-client" });
        }
        static Stopwatch speedWatch = new Stopwatch();

        static void updater_StatusChanged(object sender, StatusChangedEventArgs e)
        {
            Console.WriteLine(e.Type.ToString());
            if (e.Type == StatusChangedEnum.Start)
            {
                Console.WriteLine("Starting updater, wanted caches: dediclient");
            }
            else if (e.Type == StatusChangedEnum.Finish)
            {
                Console.WriteLine("Updater finished.");
                Bootstrap();
            }
            else if (e.Type == StatusChangedEnum.Fail)
            {
                Console.WriteLine("Updater failed. Error given: \n" + ((Exception)e.data).ToString());
            }
            else if (e.Type == StatusChangedEnum.DownloadsStart)
            {
                Console.WriteLine("Starting downloads");
            }
            else if (e.Type == StatusChangedEnum.CacheDownloading)
            {
                Console.WriteLine("Downloading update definitions");
            }
            else if (e.Type == StatusChangedEnum.CacheDownloaded)
            {
                var obj = (object[])e.data;
                var name = (String)obj[0];
                if (name == "caches.xml")
                {
                    var a = (Dictionary<string, int>)obj[1];
                    foreach (var b in a)
                    {
                        Console.WriteLine("{0}: Version {1}", b.Key, b.Value);
                    }
                }
            }
            else if (e.Type == StatusChangedEnum.FileStart)
            {
                //     pb = new ProgressBar.ProgressBar(50);
                //      progress = 0;
            }
            else if (e.Type == StatusChangedEnum.FileDownloading)
            {
                //      var obj = (object[])e.data;
                //       var name = (String)obj[0];
                //       var total = (long)obj[1];
                //      progress += (long)obj[2];
                //      pb.Update(total / progress * 100);
            }
            /*
            //random statuses are gay
            //these should be made into events :/
            if (e.DetailedStatus.StartsWith("Downloading "))
            {
                if (currentFileBytes == 0)
                {
                    //new file
                    currentSpeed = 0;
                    speedWatch.Reset(); //reset this
                    Console.WriteLine();
                    return;
                }
                if ((int)Math.Round(currentFilePercentage, 0) != (int)Math.Round(previousFilePercentage))
                {
                    if (!speedWatch.IsRunning)
                    {
                        currentSpeed = 0;
                        previousFileBytes = currentFileBytes;
                        speedWatch.Start();
                    }
                    else
                    {
                        speedWatch.Stop();
                        var diff = (int)((currentFileBytes - previousFileBytes) * 0.0001); //* 0.0001 = convert to kb
                        //speed is distance over time
                        currentSpeed = (int)(diff / speedWatch.Elapsed.TotalSeconds);
                        previousFileBytes = currentFileBytes;
                        speedWatch.Reset();
                        speedWatch.Start();
                    }
                    var output = String.Format("{0}% - {1} ({2}% - {3}/{4} - {5}kB/s)", e.Percentage, currentFile, currentFilePercentage, currentFileBytes, currentFileTotalBytes, currentSpeed);
                    Console.Write("\r");
                    var flush = "";
                    for (var i = 0; i < output.Length - currentFileBytes.ToString().Length + currentFileTotalBytes.ToString().Length + 2; i++) flush += " ";
                    Console.Write(flush); //clear the line
                    Console.Write("\r{0}", output);
                }

            }
            else if (e.DetailedStatus.StartsWith("Decom"))
            {
                Console.Write("\n{0}% - Decompressing {1} (100%)", e.Percentage, currentFile);
            }
            else if (e.DetailedStatus.StartsWith("Validating"))
            {
                Console.WriteLine("\n{0}% - Validating {1} (100%)", e.Percentage, currentFile);
            }
            else if (e.DetailedStatus.StartsWith("No Up"))
            {
                Console.WriteLine("100% - No Update Needed");
            }
            else if (e.DetailedStatus.StartsWith("Check"))
            {
                //Console.WriteLine("0% - A new dedicated server update is available.");
            }
            else if (e.DetailedStatus.StartsWith("!"))
            {
                Console.WriteLine("{0}", e.DetailedStatus.Substring(1));
            }*/
        }
        static void Bootstrap()
        {
            try
            {
                var fn = Path.Combine(Environment.CurrentDirectory, Path.Combine("bootstrap", Path.GetFileName(Assembly.GetExecutingAssembly().Location))); //problem?
                var tn = Path.GetFileName(Assembly.GetExecutingAssembly().Location);
                if (File.Exists(fn))
                {
                    if (Utilities.GetFileSHA1(fn) != Utilities.GetFileSHA1(tn))
                    {
                        //files don't match, not good!
                        if (Environment.OSVersion.Platform == PlatformID.Unix ||
                            Environment.OSVersion.Platform == PlatformID.MacOSX)
                        {
                            //can just overwrite files, simple (linux <3)
                            Log.Info("Copying files for updater updating. (linux)");
                            foreach (var file in Directory.GetFiles(Path.Combine(Environment.CurrentDirectory, "bootstrap")))
                            {
                                File.Move(file, Path.GetFileName(file));
                            }
                        }
                        else
                        {
                            //assume windows with locking filesystem (windows </3)
                            var us = Path.Combine(Environment.CurrentDirectory, "update.cmd");
                            if (File.Exists(us))
                            {
                                Log.Info("Starting update.cmd for updater updating.");
                                var psi = new ProcessStartInfo(us, "Updater.exe"); //start the updater again
                                psi.WorkingDirectory = Environment.CurrentDirectory;

                                Process.Start(psi);
                            }
                        }
                    } 
                }
            }
            catch(Exception e)
            {
                Log.Error("Bootstrap error: " + e.ToString());
            }
        }
    }
}

//-----------------------------------------------------------------------
// <copyright file="ProgressBar.cs" company="Marcus Furius Development">
//     Copyright (c) Dean Harris 2010. This is free software: you can
//     redistribute it and/or modify it under the terms of the GNU
//     General Public License as published by the Free Software
//     Foundation, either version 3 of the License, or (at your option)
//     any later version.
// </copyright>
// <author>Dean Harris</author>
//-----------------------------------------------------------------------
namespace ProgressBar
{
    /// <summary>
    /// Class to create a console progress bar
    /// </summary>
    public class ProgressBar
    {
        #region Fields

        /// <summary>
        /// The length of the last output
        /// </summary>
        private int lastOutputLength;

        /// <summary>
        /// The maximum length of the progress bar
        /// </summary>
        private int maximumWidth;

        #endregion Fields

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the  class.
        /// </summary>
        /// <param name="maximumWidth">The maximum width of the progress bar.</param>
        public ProgressBar(int maximumWidth)
        {
            this.maximumWidth = maximumWidth;
            this.Show(" [ ");
        }

        #endregion Constructors

        #region Methods

        /// <summary>
        /// Updates the progress bar with the secified percent.
        /// </summary>
        /// <param name="percent">The percent.</param>
        public void Update(double percent)
        {
            // Remove the last state
            string clear = string.Empty.PadRight(
                this.lastOutputLength,
                '\b');
            this.Show(clear);

            // Generate new state
            int width = (int)(percent / 100 * this.maximumWidth);
            int fill = this.maximumWidth - width;
            string output = string.Format(
                "{0}{1} ] {2}%",
                string.Empty.PadLeft(width, '='),
                string.Empty.PadLeft(fill, ' '),
                percent.ToString("0.0"));
            this.Show(output);
            this.lastOutputLength = output.Length;
        }

        /// <summary>
        /// Shows the specified value.
        /// </summary>
        /// <param name="value">The value.</param>
        private void Show(string value)
        {
            System.Console.Write(value);
        }

        #endregion Methods
    }
}