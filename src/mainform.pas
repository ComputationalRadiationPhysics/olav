unit mainForm;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls,
  Graphics, Dialogs, ExtCtrls, StdCtrls, ComCtrls, RichMemo, Process;

type

  { TForm1 }

  TForm1 = class( TForm )
    about: TButton;
    change: TCheckBox;
    demo: TCheckBox;
    Label1: TLabel;
    particleCount: TLabel;
    participants: TButton;
    license: TButton;
    expert: TRadioButton;
    dummyMode: TRadioGroup;
    Standard: TRadioButton;
    particleLabel: TLabel;
    textMemo: TRichMemo;
    stereo: TComboBox;
    stereoLabel: TLabel;
    Bevel1: TBevel;
    antialiasing: TComboBox;
    antialiasingLabel: TLabel;
    start: TButton;
    language: TRadioGroup;
    deutsch: TRadioButton;
    english: TRadioButton;
    fullscreen: TComboBox;
    resolutionLabel: TLabel;
    fullscreenLabel: TLabel;
    settings: TGroupBox;
    programName: TLabel;
    resolution: TComboBox;
    header: TLabel;
    lehrstuhl: TImage;
    logo: TImage;
    Shape1: TShape;
    particleBar: TTrackBar;
    tudresden: TImage;
    ogre: TImage;
    procedure aboutClick(Sender: TObject);
    procedure antialiasingChange(Sender: TObject);
    procedure changeChange(Sender: TObject);
    constructor Create(component : TComponent); override;
    procedure demoChange(Sender: TObject);
    procedure deutschChange(Sender: TObject);
    procedure englishChange(Sender: TObject);
    procedure expertChange(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure fullscreenChange(Sender: TObject);
    procedure licenseClick(Sender: TObject);
    procedure participantsClick(Sender: TObject);
    procedure particleBarChange(Sender: TObject);
    procedure resolutionChange(Sender: TObject);
    procedure StandardChange(Sender: TObject);
    procedure startClick(Sender: TObject);
    procedure stereoChange(Sender: TObject);
  private
    folder : String;
    appdata : String;
    showRTF : integer;
    first_time : integer;
    { private declarations }
  public
    procedure updateConfig;
    procedure readConfig;
    procedure readRTF;
    { public declarations }
  end;


const
  folderCount : integer = 3;
  folderPossibilty : array[0..2] of String  = ('/dist/bin/','\..\Release\','\Release\bin\');
  {$IFDEF WIN32}
  heroCall : String = 'LMC.exe';
  {$ENDIF}
  {$IFDEF LINUX}
  heroCall : String = 'LMC';
  {$ENDIF}

var
  Form1: TForm1;

implementation

{$R *.lfm}

constructor TForm1.Create(component : TComponent);
var
  AProcess: TProcess;
  myFile  : TextFile;
  content : String;
  i       : integer;
begin
  inherited;
  first_time := 1;
  {$IFDEF WIN32}
  appdata := GetEnvironmentVariableUTF8('APPDATA')+'\olav\';
  {$ENDIF}
  {$IFDEF LINUX}
  appdata := GetEnvironmentVariableUTF8('HOME')+'/.config/olav/';
  {$ENDIF}
  CreateDir(appdata);
  //Starting LMC for testing for resolutions
  AProcess := TProcess.Create(nil);
  //Searching the folder with "LMC(.exe|.i386|.amd64)".
  i := 0;
  repeat
    folder := GetCurrentDir + folderPossibilty[i];
    inc(i);
  until (FileExistsUTF8(folder + heroCall)) or (i >= folderCount);

  if not FileExistsUTF8(appdata+'ogre.cfg') then
     CopyFile(folder+'ogre.cfg',appdata+'ogre.cfg');

  AProcess.CommandLine := folder + heroCall + ' --resolutions';
  AProcess.CurrentDirectory := folder;
  AProcess.Options := AProcess.Options + [poWaitOnExit];
  AProcess.Execute;
  AProcess.Free;
  //Reading the possible resolutions
  AssignFile(myFile, appdata + 'resolutions.txt');

  Reset(myFile);
  Repeat
    Readln(myFile,content);
    resolution.Items.Add(content);
  Until Eof(myFile);
  CloseFile(myFile);
  showRTF := 0;
  readConfig;
end;

procedure TForm1.demoChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.updateConfig;
var options : array[1..100] of String;
    myFile  : TextFile;
    i,count : integer;
begin
  AssignFile(myFile, appdata + 'ogre.cfg');
  Reset(myFile);
  i := 1;
  Repeat
    count := i;
    Readln(myFile,options[i]);
    inc(i);
  Until Eof(myFile);
  CloseFile(myFile);
  AssignFile(myFile, appdata + 'ogre.cfg');
  ReWrite(myFile);
  for i:=1 to count do
  begin
    //Video Mode=
    if (LeftStr(options[i],11) = 'Video Mode=') then
      options[i] := 'Video Mode=' + resolution.items[resolution.ItemIndex];
    //FSAA=
    if (LeftStr(options[i],5) = 'FSAA=') then
      options[i] := 'FSAA=' + antialiasing.items[antialiasing.ItemIndex];
    //Full Screen=
    if (LeftStr(options[i],12) = 'Full Screen=') then
    begin
      if fullscreen.ItemIndex = 0 then
        options[i] := 'Full Screen=Yes'
      else
        options[i] := 'Full Screen=No';
    end;
    writeln(myFile,options[i]);
  end;
  CloseFile(myFile);
  AssignFile(myFile, appdata + 'starter.cfg');
  ReWrite(myFile);
  if deutsch.checked then
    writeln(myFile,'Language=German')
  else
    writeln(myFile,'Language=English');
  case stereo.ItemIndex of
    0 : writeln(myFile,'Stereo=No');
    1 : writeln(myFile,'Stereo=dualoutput');
    2 : writeln(myFile,'Stereo=redcyan');
    3 : writeln(myFile,'Stereo=yellowblue');
    4 : writeln(myFile,'Stereo=redgreen');
    5 : writeln(myFile,'Stereo=vertical');
    6 : writeln(myFile,'Stereo=horizental');
  end;
  if standard.checked then
    writeln(myFile,'Mode=Standard')
  else
    writeln(myFile,'Mode=Expert');
  if demo.checked then
    writeln(myFile,'Demo=Yes')
  else
    writeln(myFile,'Demo=No');
  if change.checked then
    writeln(myFile,'Change=Yes')
  else
    writeln(myFile,'Change=No');
  writeln(myFile,'Particles=' + inttostr(particleBar.Position));
  CloseFile(myFile);
end;

function restOfLeftString(s : String;l : integer) : String;
var i : integer;
begin
  restOfLeftString := '';
  for i:=1 to length(s) do
  begin
    if i < l then
      Continue;
    restOfLeftString := restOfLeftString + s[i];
  end;
end;

procedure TForm1.readConfig;
var options : array[1..100] of String;
    myFile  : TextFile;
    i,j,count : integer;
    line,part : String;
    LAN     : integer;
    STE     : integer;
    MODE    : integer;
    DEM     : integer;
    CHA     : integer;
begin
  AssignFile(myFile, appdata + 'ogre.cfg');
  Reset(myFile);
  i := 1;
  Repeat
    count := i;
    Readln(myFile,options[i]);
    inc(i);
  Until Eof(myFile);
  CloseFile(myFile);
  for i:=1 to count do
  begin
    //Video Mode=
    if (LeftStr(options[i],11) = 'Video Mode=') then
    begin
      part := restOfLeftString(options[i],12);
      for j:=0 to resolution.Items.Count-1 do
        if part = resolution.Items[j] then
          resolution.ItemIndex:=j;
    end;
    //FSAA=
    if (LeftStr(options[i],5) = 'FSAA=') then
    begin
      part := restOfLeftString(options[i],6);
      for j:=0 to antialiasing.Items.Count-1 do
        if part = antialiasing.Items[j] then
          antialiasing.ItemIndex:=j;
    end;
    //Full Screen=
    if (LeftStr(options[i],12) = 'Full Screen=') then
    begin
      part := restOfLeftString(options[i],13);
      if part = 'Yes' then
        fullscreen.ItemIndex := 0
      else
        fullscreen.ItemIndex := 1;
    end;
  end;
  if not FileExists(appdata + 'starter.cfg') then
    exit;
  AssignFile(myFile, appdata + 'starter.cfg');
  Reset(myFile);
  Repeat
    Readln(myFile,line);
    //Language=
    if (LeftStr(line,9) = 'Language=') then
    begin
      part := restOfLeftString(line,10);
      if part = 'German' then
        LAN := 0
      else
        LAN := 1;
    end;
    //Stereo=
    if (LeftStr(line,7) = 'Stereo=') then
    begin
      part := restOfLeftString(line,8);
      if part = 'No' then STE := 0;
      if part = 'dualoutput' then STE := 1;
      if part = 'redcyan' then STE := 2;
      if part = 'yellowblue' then STE := 3;
      if part = 'redgreen' then STE := 4;
      if part = 'vertical' then STE := 5;
      if part = 'horizental' then STE := 6;
    end;
    //Mode=
    if (LeftStr(line,5) = 'Mode=') then
    begin
      part := restOfLeftString(line,6);
      if part = 'Standard' then
        MODE := 0
      else
        MODE := 1;
    end;
    //Demo=
    if (LeftStr(line,5) = 'Demo=') then
    begin
      part := restOfLeftString(line,6);
      if part = 'Yes' then
        DEM := 1
      else
        DEM := 0;
    end;
    //Change=
    if (LeftStr(line,7) = 'Change=') then
    begin
      part := restOfLeftString(line,8);
      if part = 'Yes' then
        CHA := 1
      else
        CHA := 0;
    end;
    //Particles=
    if (LeftStr(line,10) = 'Particles=') then
    begin
      part := restOfLeftString(line,11);
      particleBar.Position := StrToInt(part);
    end;
  Until Eof(myFile);
  CloseFile(myFile);
  if LAN = 0 then
    deutsch.Checked := true
  else
    english.Checked := true;
  stereo.ItemIndex := STE;
  if MODE = 0 then
    standard.Checked := true
  else
    expert.Checked := true;
  if DEM = 1 then
    demo.Checked := true
  else
    demo.Checked := false;
  if CHA = 1 then
    change.Checked := true
  else
    change.Checked := false;
end;

procedure TForm1.readRTF;
var fs : TFileStream;
    buffer : string;
    ch : Ansichar;
begin
  buffer := '';
  {$IFDEF LINUX}
  case showRTF of
    0:
    if deutsch.checked then
      fs := TFileStream.Create('descriptions/hilfe.txt',fmOpenRead or fmShareDenyNone)
    else
      fs := TFileStream.Create('descriptions/help.txt',fmOpenRead or fmShareDenyNone);
    1:
    if deutsch.checked then
      fs := TFileStream.Create('descriptions/mitwirkende.txt',fmOpenRead or fmShareDenyNone)
    else
      fs := TFileStream.Create('descriptions/participants.txt',fmOpenRead or fmShareDenyNone);
    2:
    if deutsch.checked then
      fs := TFileStream.Create('descriptions/lizenzen.txt',fmOpenRead or fmShareDenyNone)
    else
      fs := TFileStream.Create('descriptions/licenses.txt',fmOpenRead or fmShareDenyNone);
  end;
  ch := 'a';
  while (fs.Read( ch, 1) = 1) do
  begin
    buffer := buffer + ch;
  end;
  textMemo.text := buffer;
  fs.free;
  {$ENDIF}
  {$IFDEF WIN32}
  case showRTF of
    0:
    if deutsch.checked then
      fs := TFileStream.Create('descriptions\hilfe.rtf',fmOpenRead or fmShareDenyNone)
    else
      fs := TFileStream.Create('descriptions\help.rtf',fmOpenRead or fmShareDenyNone);
    1:
    if deutsch.checked then
      fs := TFileStream.Create('descriptions\mitwirkende.rtf',fmOpenRead or fmShareDenyNone)
    else
      fs := TFileStream.Create('descriptions\participants.rtf',fmOpenRead or fmShareDenyNone);
    2:
    if deutsch.checked then
      fs := TFileStream.Create('descriptions\lizenzen.rtf',fmOpenRead or fmShareDenyNone)
    else
      fs := TFileStream.Create('descriptions\licenses.rtf',fmOpenRead or fmShareDenyNone);
  end;
  textMemo.LoadRichText(fs);
  fs.free;
  {$ENDIF}
end;

procedure TForm1.deutschChange(Sender: TObject);
begin
  updateConfig;
  if deutsch.Checked then
  begin
    programName.Caption       := 'Optical Laser & electron Accelerator Visualization';
    header.Caption            := 'Hochleistungsstrahlenquellensimulation des HZDR';
    settings.Caption          := 'Einstellungen';
    language.Caption          := 'Sprache';
    resolutionLabel.Caption   := 'Auflösung:';
    fullscreenLabel.Caption   := 'Vollbild:';
    fullscreen.Items[0]       := 'Ja';
    fullscreen.Items[1]       := 'Nein';
    antialiasingLabel.Caption := 'Kantenglättung:';
    stereoLabel.Caption       := 'Stereoskopie:';
    stereo.Items[0]           := 'Aus';
    stereo.Items[1]           := 'Dual Output';
    stereo.Items[2]           := 'Rot-Cyan';
    stereo.Items[3]           := 'Gelb-Blau';
    stereo.Items[4]           := 'Rot-Grün';
    stereo.Items[5]           := 'Vertikal';
    stereo.Items[6]           := 'Horizental';
    about.Caption             := 'Über';
    participants.Caption      := 'Mitwirkende';
    license.Caption           := 'Lizenzen';
    standard.Caption          := 'Beginner';
    expert.Caption            := 'Experte';
    dummyMode.Caption         := 'Modus';
    change.Caption            := 'Wechsel';
    particleLabel.Caption     := 'Elbe Partikel:';
  end;
  readRTF;
end;

procedure TForm1.englishChange(Sender: TObject);
begin
  updateConfig;
  if english.Checked then
  begin
    programName.Caption       := 'Optical Laser & electron Accelerator Visualization';
    header.Caption            := 'High power radiation source simulation of the HZDR';
    settings.Caption          := 'Settings';
    language.Caption          := 'Language';
    resolutionLabel.Caption   := 'Resolution:';
    fullscreenLabel.Caption   := 'Fullscreen:';
    fullscreen.Items[0]       := 'Yes';
    fullscreen.Items[1]       := 'No';
    antialiasingLabel.Caption := 'Antialiasing:';
    stereoLabel.Caption       := 'Stereoscopic:';
    stereo.Items[0]           := 'Off';
    stereo.Items[1]           := 'Dual Output';
    stereo.Items[2]           := 'Red-Cyan';
    stereo.Items[3]           := 'Yellow-Blue';
    stereo.Items[4]           := 'Red-Green';
    stereo.Items[5]           := 'Vertical';
    stereo.Items[6]           := 'Horizental';
    about.Caption             := 'About';
    participants.Caption      := 'Participants';
    license.Caption           := 'Licenses';
    standard.Caption          := 'beginner';
    expert.Caption            := 'expert';
    dummyMode.Caption         := 'Mode';
    change.Caption            := 'Change';
    particleLabel.Caption     := 'Elbe particles:';
  end;
  readRTF;
end;

procedure TForm1.expertChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.FormShow(Sender: TObject);
begin
  readRTF;
end;

procedure TForm1.fullscreenChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.licenseClick(Sender: TObject);
begin
  showRTF := 2;
  readRTF;
end;

procedure TForm1.participantsClick(Sender: TObject);
begin
  showRTF := 1;
  readRTF;
end;

procedure TForm1.particleBarChange(Sender: TObject);
begin
  if first_time = 1 then
      first_time := 0
  else
      updateConfig;
  particleCount.Caption:=IntToStr(particleBar.Position);
end;

procedure TForm1.resolutionChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.StandardChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.antialiasingChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.changeChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.aboutClick(Sender: TObject);
begin
  showRTF := 0;
  readRTF;
end;

procedure TForm1.stereoChange(Sender: TObject);
begin
  updateConfig;
end;

procedure TForm1.startClick(Sender: TObject);
var
  AProcess: TProcess;
begin
  //Starting LMC
  AProcess := TProcess.Create(nil);
  AProcess.CommandLine := folder + heroCall + ' -n';
  AProcess.CurrentDirectory := folder;
  case stereo.ItemIndex of
    1 : AProcess.CommandLine := AProcess.CommandLine + ' -s -m dualoutput';
    2 : AProcess.CommandLine := AProcess.CommandLine + ' -s -m redcyan';
    3 : AProcess.CommandLine := AProcess.CommandLine + ' -s -m yellowblue';
    4 : AProcess.CommandLine := AProcess.CommandLine + ' -s -m redgreen';
    5 : AProcess.CommandLine := AProcess.CommandLine + ' -s -m vertical';
    6 : AProcess.CommandLine := AProcess.CommandLine + ' -s -m horizental';
  end;
  if deutsch.Checked then
     AProcess.CommandLine := AProcess.CommandLine + ' --german';
  if Standard.Checked then
     AProcess.CommandLine := AProcess.CommandLine + ' --simple';
  if demo.Checked then
     AProcess.CommandLine := AProcess.CommandLine + ' --demo';
  if change.Checked then
     AProcess.CommandLine := AProcess.CommandLine + ' --change';

  AProcess.CommandLine := AProcess.CommandLine + ' --particles=' + IntToStr(particleBar.Position);

  AProcess.Options := AProcess.Options + [poWaitOnExit];
  AProcess.Execute;
  AProcess.Free;
  Close;
end;


end.

