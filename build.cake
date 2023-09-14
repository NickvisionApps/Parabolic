const string appId = "org.nickvision.tubeconverter";
const string projectName = "NickvisionTubeConverter";
const string shortName = "parabolic";
readonly string[] projectsToBuild = new string[] { "GNOME" };

if (FileExists("CakeScripts/main.cake"))
{
    #load local:?path=CakeScripts/main.cake
}
else
{
    throw new CakeException("Failed to load main script.");
}