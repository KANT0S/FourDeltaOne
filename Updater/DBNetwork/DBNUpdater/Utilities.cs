using System;
using System.IO;
using System.Net;
using System.Security.Cryptography;

namespace DBNUpdater
{
    public static class Utilities
    {
        public static string GetFileSHA1(string fileName)
        {
            return GetFileHash(new SHA1Managed(), fileName).ToUpper();
        }

        private static string GetFileHash(HashAlgorithm algorithm, string fileName)
        {
            var stream = File.OpenRead(fileName);
            var hashResult = algorithm.ComputeHash(stream);
            stream.Close();

            return BitConverter.ToString(hashResult).Replace("-", "");
        }

        public static bool URLExists(string url)
        {
            try
            {
                ServicePointManager.Expect100Continue = false;

                var request = (HttpWebRequest)WebRequest.Create(url);
                request.Method = "HEAD";
                request.Timeout = 15000;

                var response = (HttpWebResponse)request.GetResponse();
                bool exists = ((int)response.StatusCode < 300);
                response.Close();

                return exists;
            }
            catch (WebException e)
            {
                if (((HttpWebResponse)e.Response).StatusCode == HttpStatusCode.NotFound)
                {
                    Log.Debug("URL not found: " + url);
                    return false;
                }
                else
                {
                    Log.Debug(url + ": " + e.ToString());
                    return false;
                }
            }
            catch(Exception)
            {
                return false;
            }
        }

        public static string CombinePaths(string first, params string[] others)
        {
            // from http://stackoverflow.com/questions/760612/how-can-path-combine-be-used-with-more-than-2-arguments

            // Put error checking in here :)
            string path = first;
            foreach (string section in others)
            {
                path = Path.Combine(path, section);
            }
            return path;
        }
    }
}
