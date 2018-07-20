// 
//  ____  _     __  __      _        _ 
// |  _ \| |__ |  \/  | ___| |_ __ _| |
// | | | | '_ \| |\/| |/ _ \ __/ _` | |
// | |_| | |_) | |  | |  __/ || (_| | |
// |____/|_.__/|_|  |_|\___|\__\__,_|_|
//
// Auto-generated from xnp on 2012-06-12 08:24:47Z.
// Please visit http://code.google.com/p/dblinq2007/ for more information.
//
using System;
using System.ComponentModel;
using System.Data;
#if MONO_STRICT
	using System.Data.Linq;
#else   // MONO_STRICT
	using DbLinq.Data.Linq;
	using DbLinq.Vendor;
#endif  // MONO_STRICT
	using System.Data.Linq.Mapping;
using System.Diagnostics;


public partial class XNP : DataContext
{
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		#endregion
	
	
	public XNP(string connectionString) : 
			base(connectionString)
	{
		this.OnCreated();
	}
	
	public XNP(string connection, MappingSource mappingSource) : 
			base(connection, mappingSource)
	{
		this.OnCreated();
	}
	
	public XNP(IDbConnection connection, MappingSource mappingSource) : 
			base(connection, mappingSource)
	{
		this.OnCreated();
	}
	
	public Table<ExternalPlatforms> ExternalPlatforms
	{
		get
		{
			return this.GetTable<ExternalPlatforms>();
		}
	}
	
	public Table<Friends> Friends
	{
		get
		{
			return this.GetTable<Friends>();
		}
	}
	
	public Table<IW4Profiles> IW4Profiles
	{
		get
		{
			return this.GetTable<IW4Profiles>();
		}
	}
	
	public Table<LicenseKeys> LicenseKeys
	{
		get
		{
			return this.GetTable<LicenseKeys>();
		}
	}
	
	public Table<Matches> Matches
	{
		get
		{
			return this.GetTable<Matches>();
		}
	}
	
	public Table<MatchUsers> MatchUsers
	{
		get
		{
			return this.GetTable<MatchUsers>();
		}
	}
}

#region Start MONO_STRICT
#if MONO_STRICT

public partial class XNP
{
	
	public XNP(IDbConnection connection) : 
			base(connection)
	{
		this.OnCreated();
	}
}
#region End MONO_STRICT
	#endregion
#else     // MONO_STRICT

public partial class XNP
{
	
	public XNP(IDbConnection connection) : 
			base(connection, new DbLinq.MySql.MySqlVendor())
	{
		this.OnCreated();
	}
	
	public XNP(IDbConnection connection, IVendor sqlDialect) : 
			base(connection, sqlDialect)
	{
		this.OnCreated();
	}
	
	public XNP(IDbConnection connection, MappingSource mappingSource, IVendor sqlDialect) : 
			base(connection, mappingSource, sqlDialect)
	{
		this.OnCreated();
	}
}
#region End Not MONO_STRICT
	#endregion
#endif     // MONO_STRICT
#endregion

[Table(Name="xnp.external_platforms")]
public partial class ExternalPlatforms : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
{
	
	private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
	
	private sbyte _platformAuthenticated;
	
	private long _platformID;
	
	private string _platformType;
	
	private int _userID;
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnPlatformAuthenticatedChanged();
		
		partial void OnPlatformAuthenticatedChanging(sbyte value);
		
		partial void OnPlatformIDChanged();
		
		partial void OnPlatformIDChanging(long value);
		
		partial void OnPlatformTypeChanged();
		
		partial void OnPlatformTypeChanging(string value);
		
		partial void OnUserIDChanged();
		
		partial void OnUserIDChanging(int value);
		#endregion
	
	
	public ExternalPlatforms()
	{
		this.OnCreated();
	}
	
	[Column(Storage="_platformAuthenticated", Name="platform_authenticated", DbType="tinyint(1)", AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public sbyte PlatformAuthenticated
	{
		get
		{
			return this._platformAuthenticated;
		}
		set
		{
			if ((_platformAuthenticated != value))
			{
				this.OnPlatformAuthenticatedChanging(value);
				this.SendPropertyChanging();
				this._platformAuthenticated = value;
				this.SendPropertyChanged("PlatformAuthenticated");
				this.OnPlatformAuthenticatedChanged();
			}
		}
	}
	
	[Column(Storage="_platformID", Name="platform_id", DbType="bigint(20) unsigned", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public long PlatformID
	{
		get
		{
			return this._platformID;
		}
		set
		{
			if ((_platformID != value))
			{
				this.OnPlatformIDChanging(value);
				this.SendPropertyChanging();
				this._platformID = value;
				this.SendPropertyChanged("PlatformID");
				this.OnPlatformIDChanged();
			}
		}
	}
	
	[Column(Storage="_platformType", Name="platform_type", DbType="varchar(20)", AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public string PlatformType
	{
		get
		{
			return this._platformType;
		}
		set
		{
			if (((_platformType == value) 
						== false))
			{
				this.OnPlatformTypeChanging(value);
				this.SendPropertyChanging();
				this._platformType = value;
				this.SendPropertyChanged("PlatformType");
				this.OnPlatformTypeChanged();
			}
		}
	}
	
	[Column(Storage="_userID", Name="user_id", DbType="int", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public int UserID
	{
		get
		{
			return this._userID;
		}
		set
		{
			if ((_userID != value))
			{
				this.OnUserIDChanging(value);
				this.SendPropertyChanging();
				this._userID = value;
				this.SendPropertyChanged("UserID");
				this.OnUserIDChanged();
			}
		}
	}
	
	public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
	
	public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
	
	protected virtual void SendPropertyChanging()
	{
		System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
		if ((h != null))
		{
			h(this, emptyChangingEventArgs);
		}
	}
	
	protected virtual void SendPropertyChanged(string propertyName)
	{
		System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
		if ((h != null))
		{
			h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
		}
	}
}

[Table(Name="xnp.friends")]
public partial class Friends : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
{
	
	private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
	
	private System.Nullable<sbyte> _friend;
	
	private int _friendID;
	
	private System.Nullable<sbyte> _isExternal;
	
	private int _userID;
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnFriendChanged();
		
		partial void OnFriendChanging(System.Nullable<sbyte> value);
		
		partial void OnFriendIDChanged();
		
		partial void OnFriendIDChanging(int value);
		
		partial void OnIsExternalChanged();
		
		partial void OnIsExternalChanging(System.Nullable<sbyte> value);
		
		partial void OnUserIDChanged();
		
		partial void OnUserIDChanging(int value);
		#endregion
	
	
	public Friends()
	{
		this.OnCreated();
	}
	
	[Column(Storage="_friend", Name="friend", DbType="tinyint(1)", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public System.Nullable<sbyte> Friend
	{
		get
		{
			return this._friend;
		}
		set
		{
			if ((_friend != value))
			{
				this.OnFriendChanging(value);
				this.SendPropertyChanging();
				this._friend = value;
				this.SendPropertyChanged("Friend");
				this.OnFriendChanged();
			}
		}
	}
	
	[Column(Storage="_friendID", Name="friend_id", DbType="int(10)", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public int FriendID
	{
		get
		{
			return this._friendID;
		}
		set
		{
			if ((_friendID != value))
			{
				this.OnFriendIDChanging(value);
				this.SendPropertyChanging();
				this._friendID = value;
				this.SendPropertyChanged("FriendID");
				this.OnFriendIDChanged();
			}
		}
	}
	
	[Column(Storage="_isExternal", Name="is_external", DbType="tinyint(1)", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public System.Nullable<sbyte> IsExternal
	{
		get
		{
			return this._isExternal;
		}
		set
		{
			if ((_isExternal != value))
			{
				this.OnIsExternalChanging(value);
				this.SendPropertyChanging();
				this._isExternal = value;
				this.SendPropertyChanged("IsExternal");
				this.OnIsExternalChanged();
			}
		}
	}
	
	[Column(Storage="_userID", Name="user_id", DbType="int(10)", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public int UserID
	{
		get
		{
			return this._userID;
		}
		set
		{
			if ((_userID != value))
			{
				this.OnUserIDChanging(value);
				this.SendPropertyChanging();
				this._userID = value;
				this.SendPropertyChanged("UserID");
				this.OnUserIDChanged();
			}
		}
	}
	
	public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
	
	public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
	
	protected virtual void SendPropertyChanging()
	{
		System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
		if ((h != null))
		{
			h(this, emptyChangingEventArgs);
		}
	}
	
	protected virtual void SendPropertyChanged(string propertyName)
	{
		System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
		if ((h != null))
		{
			h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
		}
	}
}

[Table(Name="xnp.iw4_profiles")]
public partial class IW4Profiles : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
{
	
	private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
	
	private string _cardIcon;
	
	private string _cardTitle;
	
	private System.Nullable<int> _experience;
	
	private System.Nullable<sbyte> _prestige;
	
	private int _userID;
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnCardIconChanged();
		
		partial void OnCardIconChanging(string value);
		
		partial void OnCardTitleChanged();
		
		partial void OnCardTitleChanging(string value);
		
		partial void OnExperienceChanged();
		
		partial void OnExperienceChanging(System.Nullable<int> value);
		
		partial void OnPrestigeChanged();
		
		partial void OnPrestigeChanging(System.Nullable<sbyte> value);
		
		partial void OnUserIDChanged();
		
		partial void OnUserIDChanging(int value);
		#endregion
	
	
	public IW4Profiles()
	{
		this.OnCreated();
	}
	
	[Column(Storage="_cardIcon", Name="cardicon", DbType="varchar(60)", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public string CardIcon
	{
		get
		{
			return this._cardIcon;
		}
		set
		{
			if (((_cardIcon == value) 
						== false))
			{
				this.OnCardIconChanging(value);
				this.SendPropertyChanging();
				this._cardIcon = value;
				this.SendPropertyChanged("CardIcon");
				this.OnCardIconChanged();
			}
		}
	}
	
	[Column(Storage="_cardTitle", Name="cardtitle", DbType="varchar(60)", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public string CardTitle
	{
		get
		{
			return this._cardTitle;
		}
		set
		{
			if (((_cardTitle == value) 
						== false))
			{
				this.OnCardTitleChanging(value);
				this.SendPropertyChanging();
				this._cardTitle = value;
				this.SendPropertyChanged("CardTitle");
				this.OnCardTitleChanged();
			}
		}
	}
	
	[Column(Storage="_experience", Name="experience", DbType="int", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public System.Nullable<int> Experience
	{
		get
		{
			return this._experience;
		}
		set
		{
			if ((_experience != value))
			{
				this.OnExperienceChanging(value);
				this.SendPropertyChanging();
				this._experience = value;
				this.SendPropertyChanged("Experience");
				this.OnExperienceChanged();
			}
		}
	}
	
	[Column(Storage="_prestige", Name="prestige", DbType="tinyint(4)", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public System.Nullable<sbyte> Prestige
	{
		get
		{
			return this._prestige;
		}
		set
		{
			if ((_prestige != value))
			{
				this.OnPrestigeChanging(value);
				this.SendPropertyChanging();
				this._prestige = value;
				this.SendPropertyChanged("Prestige");
				this.OnPrestigeChanged();
			}
		}
	}
	
	[Column(Storage="_userID", Name="user_id", DbType="int", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public int UserID
	{
		get
		{
			return this._userID;
		}
		set
		{
			if ((_userID != value))
			{
				this.OnUserIDChanging(value);
				this.SendPropertyChanging();
				this._userID = value;
				this.SendPropertyChanged("UserID");
				this.OnUserIDChanged();
			}
		}
	}
	
	public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
	
	public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
	
	protected virtual void SendPropertyChanging()
	{
		System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
		if ((h != null))
		{
			h(this, emptyChangingEventArgs);
		}
	}
	
	protected virtual void SendPropertyChanged(string propertyName)
	{
		System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
		if ((h != null))
		{
			h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
		}
	}
}

[Table(Name="xnp.licensekeys")]
public partial class LicenseKeys : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
{
	
	private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
	
	private uint _id;
	
	private string _key;
	
	private System.Nullable<int> _type;
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnIDChanged();
		
		partial void OnIDChanging(uint value);
		
		partial void OnKeyChanged();
		
		partial void OnKeyChanging(string value);
		
		partial void OnTypeChanged();
		
		partial void OnTypeChanging(System.Nullable<int> value);
		#endregion
	
	
	public LicenseKeys()
	{
		this.OnCreated();
	}
	
	[Column(Storage="_id", Name="id", DbType="int unsigned", IsPrimaryKey=true, IsDbGenerated=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public uint ID
	{
		get
		{
			return this._id;
		}
		set
		{
			if ((_id != value))
			{
				this.OnIDChanging(value);
				this.SendPropertyChanging();
				this._id = value;
				this.SendPropertyChanged("ID");
				this.OnIDChanged();
			}
		}
	}
	
	[Column(Storage="_key", Name="key", DbType="varchar(24)", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public string Key
	{
		get
		{
			return this._key;
		}
		set
		{
			if (((_key == value) 
						== false))
			{
				this.OnKeyChanging(value);
				this.SendPropertyChanging();
				this._key = value;
				this.SendPropertyChanged("Key");
				this.OnKeyChanged();
			}
		}
	}
	
	[Column(Storage="_type", Name="type", DbType="int", AutoSync=AutoSync.Never)]
	[DebuggerNonUserCode()]
	public System.Nullable<int> Type
	{
		get
		{
			return this._type;
		}
		set
		{
			if ((_type != value))
			{
				this.OnTypeChanging(value);
				this.SendPropertyChanging();
				this._type = value;
				this.SendPropertyChanged("Type");
				this.OnTypeChanged();
			}
		}
	}
	
	public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
	
	public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
	
	protected virtual void SendPropertyChanging()
	{
		System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
		if ((h != null))
		{
			h(this, emptyChangingEventArgs);
		}
	}
	
	protected virtual void SendPropertyChanged(string propertyName)
	{
		System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
		if ((h != null))
		{
			h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
		}
	}
}

[Table(Name="xnp.matches")]
public partial class Matches : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
{
	
	private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
	
	private string _matchFilename;
	
	private string _matchGameType;
	
	private string _matchGuiD;
	
	private string _matchMap;
	
	private int _matchTime;
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnMatchFilenameChanged();
		
		partial void OnMatchFilenameChanging(string value);
		
		partial void OnMatchGameTypeChanged();
		
		partial void OnMatchGameTypeChanging(string value);
		
		partial void OnMatchGuiDChanged();
		
		partial void OnMatchGuiDChanging(string value);
		
		partial void OnMatchMapChanged();
		
		partial void OnMatchMapChanging(string value);
		
		partial void OnMatchTimeChanged();
		
		partial void OnMatchTimeChanging(int value);
		#endregion
	
	
	public Matches()
	{
		this.OnCreated();
	}
	
	[Column(Storage="_matchFilename", Name="match_filename", DbType="varchar(255)", AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public string MatchFilename
	{
		get
		{
			return this._matchFilename;
		}
		set
		{
			if (((_matchFilename == value) 
						== false))
			{
				this.OnMatchFilenameChanging(value);
				this.SendPropertyChanging();
				this._matchFilename = value;
				this.SendPropertyChanged("MatchFilename");
				this.OnMatchFilenameChanged();
			}
		}
	}
	
	[Column(Storage="_matchGameType", Name="match_gametype", DbType="char(10)", AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public string MatchGameType
	{
		get
		{
			return this._matchGameType;
		}
		set
		{
			if (((_matchGameType == value) 
						== false))
			{
				this.OnMatchGameTypeChanging(value);
				this.SendPropertyChanging();
				this._matchGameType = value;
				this.SendPropertyChanged("MatchGameType");
				this.OnMatchGameTypeChanged();
			}
		}
	}
	
	[Column(Storage="_matchGuiD", Name="match_guid", DbType="char(40)", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public string MatchGuiD
	{
		get
		{
			return this._matchGuiD;
		}
		set
		{
			if (((_matchGuiD == value) 
						== false))
			{
				this.OnMatchGuiDChanging(value);
				this.SendPropertyChanging();
				this._matchGuiD = value;
				this.SendPropertyChanged("MatchGuiD");
				this.OnMatchGuiDChanged();
			}
		}
	}
	
	[Column(Storage="_matchMap", Name="match_map", DbType="varchar(40)", AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public string MatchMap
	{
		get
		{
			return this._matchMap;
		}
		set
		{
			if (((_matchMap == value) 
						== false))
			{
				this.OnMatchMapChanging(value);
				this.SendPropertyChanging();
				this._matchMap = value;
				this.SendPropertyChanged("MatchMap");
				this.OnMatchMapChanged();
			}
		}
	}
	
	[Column(Storage="_matchTime", Name="match_time", DbType="int", AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public int MatchTime
	{
		get
		{
			return this._matchTime;
		}
		set
		{
			if ((_matchTime != value))
			{
				this.OnMatchTimeChanging(value);
				this.SendPropertyChanging();
				this._matchTime = value;
				this.SendPropertyChanged("MatchTime");
				this.OnMatchTimeChanged();
			}
		}
	}
	
	public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
	
	public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
	
	protected virtual void SendPropertyChanging()
	{
		System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
		if ((h != null))
		{
			h(this, emptyChangingEventArgs);
		}
	}
	
	protected virtual void SendPropertyChanged(string propertyName)
	{
		System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
		if ((h != null))
		{
			h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
		}
	}
}

[Table(Name="xnp.match_users")]
public partial class MatchUsers : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
{
	
	private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
	
	private string _matchID;
	
	private int _matchUser;
	
	#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnMatchIDChanged();
		
		partial void OnMatchIDChanging(string value);
		
		partial void OnMatchUserChanged();
		
		partial void OnMatchUserChanging(int value);
		#endregion
	
	
	public MatchUsers()
	{
		this.OnCreated();
	}
	
	[Column(Storage="_matchID", Name="match_id", DbType="char(40)", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public string MatchID
	{
		get
		{
			return this._matchID;
		}
		set
		{
			if (((_matchID == value) 
						== false))
			{
				this.OnMatchIDChanging(value);
				this.SendPropertyChanging();
				this._matchID = value;
				this.SendPropertyChanged("MatchID");
				this.OnMatchIDChanged();
			}
		}
	}
	
	[Column(Storage="_matchUser", Name="match_user", DbType="int", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
	[DebuggerNonUserCode()]
	public int MatchUser
	{
		get
		{
			return this._matchUser;
		}
		set
		{
			if ((_matchUser != value))
			{
				this.OnMatchUserChanging(value);
				this.SendPropertyChanging();
				this._matchUser = value;
				this.SendPropertyChanged("MatchUser");
				this.OnMatchUserChanged();
			}
		}
	}
	
	public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
	
	public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
	
	protected virtual void SendPropertyChanging()
	{
		System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
		if ((h != null))
		{
			h(this, emptyChangingEventArgs);
		}
	}
	
	protected virtual void SendPropertyChanged(string propertyName)
	{
		System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
		if ((h != null))
		{
			h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
		}
	}
}
