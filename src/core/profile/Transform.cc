#include "ach/core/profile/Transform.hh"

#include "ach/util/Commons.hh"
#include "ach/core/platform/OSType.hh"
#include <list>
#include <regex>
#include <iostream>

namespace Alicorn
{
namespace Profile
{

cJSON *
transformProfile(const cJSON *src)
{
  // Currently there are 3 generations of profile:
  // GEN 3: 1.19+ (ACH Base)
  // GEN 2: 1.13-1.19
  // GEN 1: 1.12-
  cJSON *srcObj = cJSON_Duplicate(src, true);

  // GEN 1: Key minecraftArguments
  if(cJSON_HasObjectItem(srcObj, "minecraftArguments"))
    {
      // Convert to GEN 2
      // Basically resolve game arguments
      cJSON_AddBoolToObject(srcObj, "isLegacy", 1); // For ourselves
      cJSON *arguments = cJSON_CreateArray();
      cJSON *args = cJSON_GetObjectItem(srcObj, "minecraftArguments");
      if(cJSON_IsString(args))
        {
          std::string originalArgs = cJSON_GetStringValue(args);
          auto spArgs = Commons::splitStr(originalArgs, " ");
          for(auto &a : spArgs)
            {
              cJSON *argStr = cJSON_CreateString(a.c_str());
              cJSON_AddItemToArray(arguments, argStr);
            }
          cJSON *whObj
              = cJSON_Parse("{\"rules\":[{\"action\":\"allow\",\"features\":{"
                            "\"has_custom_resolution\":true}}],\"value\": "
                            "[\"--width\",\"${resolution_width}\",\"--"
                            "height\",\"${resolution_height}\"]}");
          cJSON_AddItemToArray(arguments, whObj);

          cJSON *demoObj = cJSON_Parse(
              "{\"rules\":[{\"action\":\"allow\",\"features\":{\"is_demo_"
              "user\":true}}],\"value\":\"--demo\"},");
          cJSON_AddItemToArray(arguments, demoObj);
        }

      cJSON *argumentsBase = cJSON_CreateObject();
      cJSON_AddItemToObject(argumentsBase, "game", arguments);

      cJSON *jvmObj = cJSON_Parse(
          "[{\"rules\":[{\"action\":"
          "\"allow\",\"os\":{\"arch\":\"x86\"}}],\"value\":\"-Xss1M\"},\"-"
          "Djava.library.path=${natives_directory}\",\"-Dminecraft.launcher."
          "brand=${launcher_name}\",\"-Dminecraft.launcher.version=${launcher_"
          "version}\",\"-cp\",\"${classpath}\"]");
      cJSON_AddItemToObject(argumentsBase, "jvm", jvmObj);

      cJSON_AddItemToObject(srcObj, "arguments", argumentsBase);
    }

  // GEN 2: Key classifiers
  cJSON *libraries = cJSON_GetObjectItem(srcObj, "libraries");
  if(cJSON_IsArray(libraries))
    {
      std::list<cJSON *> libsToAdd;
      int len = cJSON_GetArraySize(libraries);
      for(int i = 0; i < len; i++)
        {
          cJSON *currentLib = cJSON_GetArrayItem(libraries, i);
          cJSON *downloads = cJSON_GetObjectItem(currentLib, "downloads");
          if(cJSON_IsObject(downloads))
            {
              cJSON *natives = cJSON_GetObjectItem(currentLib, "natives");
              if(cJSON_IsObject(natives))
                {
                  std::string k;
                  if(Platform::OS_TYPE == Platform::OS_UNIX)
                    {
                      k = "linux";
                    }
                  else if(Platform::OS_TYPE == Platform::OS_DARWIN)
                    {
                      k = "osx";
                    }
                  else if(Platform::OS_TYPE == Platform::OS_MSDOS)
                    {
                      k = "windows";
                    }
                  cJSON *nativesKey = cJSON_GetObjectItem(natives, k.c_str());
                  if(cJSON_IsString(nativesKey))
                    {
                      std::string nk = cJSON_GetStringValue(nativesKey);
                      // This requires ${arch} to be replaced
                      std::regex arch("\\$\\{arch\\}");
                      std::string bits;
#if defined(__x86_64) || defined(__amd64) || defined(_WIN64)                  \
    || defined(__MINGW64__) || defined(__arm64) || defined(__aarch64__)
                      bits = "64";
#else
                      bits = "32"
#endif
                      nk = std::regex_replace(nk, arch, bits);
                      cJSON *classifiers
                          = cJSON_GetObjectItem(downloads, "classifiers");
                      if(cJSON_IsObject(classifiers))
                        {
                          cJSON *classifiersCopy
                              = cJSON_Duplicate(classifiers, true);
                          cJSON_DeleteItemFromObject(downloads, "classifiers");
                          cJSON *nativesLib = cJSON_GetObjectItem(
                              classifiersCopy, nk.c_str()); // An artifact
                          cJSON *newLib = cJSON_CreateObject();
                          cJSON *newLibDownloads = cJSON_CreateObject();
                          cJSON_AddItemReferenceToObject(
                              newLibDownloads, "artifact", nativesLib);

                          cJSON *originalName
                              = cJSON_GetObjectItem(currentLib, "name");
                          cJSON *originalRules
                              = cJSON_GetObjectItem(currentLib, "rules");
                          if(cJSON_IsArray(originalRules))
                            {
                              cJSON *newRule
                                  = cJSON_Duplicate(originalRules, true);
                              cJSON_AddItemToObject(newLib, "rules", newRule);
                              cJSON_DeleteItemFromObject(currentLib, "rules");
                            }
                          if(cJSON_IsString(originalName))
                            {
                              std::string name
                                  = std::string(
                                        cJSON_GetStringValue(originalName))
                                    + ":" + nk;
                              cJSON_AddStringToObject(newLib, "name",
                                                      name.c_str());
                              cJSON_AddItemToObject(newLib, "downloads",
                                                    newLibDownloads);
                              libsToAdd.push_back(newLib);
                            }
                          if(!cJSON_HasObjectItem(downloads, "artifact"))
                            {
                              // Nothing left
                              cJSON_DeleteItemFromArray(libraries, i);
                              i--;
                            }
                        }
                    }
                  // Find suitable one and create new
                }
            }
        }

      for(auto &p : libsToAdd)
        {
          cJSON_AddItemToArray(libraries, p);
        }
    }
  std::string dat = cJSON_Print(srcObj);
  return srcObj;
}

}
}
