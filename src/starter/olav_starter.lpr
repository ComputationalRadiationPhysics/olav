program olav_starter;
{$IFDEF WIN32}
//{$R lazarus.rc}
{$ENDIF}

{$mode objfpc}{$H+}
{$codepage utf8}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, mainForm;

{$R *.res}

begin
  //RequireDerivedFormResource := True;
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.

