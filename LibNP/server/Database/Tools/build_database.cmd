dbmetal /provider:MySql /conn "Server=localhost;Database=xnp;Uid=xnp;Pwd=xnp" /dbml:xnp.dbml
dbmetal /code:xnp.cs xnp.dbml

copy /y xnp.cs ..\..\NPServer\Database.cs