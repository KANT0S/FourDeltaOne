using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;

using DbLinq.Data.Linq;
using MySql.Data.MySqlClient;

public partial class XNP : DataContext
{
    public static XNP Create()
    {
        var connectionString = ConfigurationManager.AppSettings["connectionString"];

        try
        {
            return new XNP(new MySqlConnection(connectionString));
        }
        catch (Exception e)
        {
            NPx.Log.Error("Could not connect to the database. Exception: " + e.ToString());
            return null;
        }
    }
}
