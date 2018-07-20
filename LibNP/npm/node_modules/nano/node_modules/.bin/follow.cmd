@IF EXIST "%~dp0\node.exe" (
  "%~dp0\node.exe"  "%~dp0\..\follow\cli.js" %*
) ELSE (
  node  "%~dp0\..\follow\cli.js" %*
)