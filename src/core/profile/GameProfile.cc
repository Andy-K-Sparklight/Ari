#include "ach/core/profile/GameProfile.hh"

#include <iostream>
#include "ach/util/Commons.hh"

namespace Alicorn
{

namespace Profile
{

template <class T>
static void
parseList(std::list<T> &target, cJSON *src)
{
  if(cJSON_IsArray(src))
    {
      int length = cJSON_GetArraySize(src);
      for(int i = 0; i < length; i++)
        {
          cJSON *currentItem = cJSON_GetArrayItem(src, i);
          T item(currentItem);
          target.push_back(item);
        }
    }
}

Rule::Rule(const cJSON *src)
{
  if(cJSON_IsObject(src))
    {
      cJSON *actionItem = cJSON_GetObjectItem(src, "action");

      if(cJSON_IsBool(actionItem))
        {
          action = cJSON_IsTrue(actionItem);
        }

      // Criteria
      cJSON *featuresItem = cJSON_GetObjectItem(src, "features");
      cJSON *osItem = cJSON_GetObjectItem(src, "os");
      if(cJSON_IsObject(featuresItem))
        {
          if(cJSON_HasObjectItem(featuresItem, "is_demo_user"))
            {
              criteria = CR_FE_DEMO;
              condition = "";
            }
          if(cJSON_HasObjectItem(featuresItem, "has_custom_resolution"))
            {
              criteria = CR_FE_CRES;
              condition = "";
            }
        }
      else if(cJSON_IsObject(osItem))
        {
          cJSON *osNameItem = cJSON_GetObjectItem(osItem, "name");
          cJSON *osVersionItem = cJSON_GetObjectItem(osItem, "version");
          cJSON *osArchItem = cJSON_GetObjectItem(osItem, "arch");

          if(cJSON_IsString(osNameItem))
            {
              criteria = CR_OS_NAME;
              condition = cJSON_GetStringValue(osNameItem);
            }

          // This one must put below the name, or it will be replaced
          if(cJSON_IsString(osVersionItem))
            {
              criteria = CR_OS_VERSION;
              condition = cJSON_GetStringValue(osVersionItem);
            }

          if(cJSON_IsString(osArchItem))
            {
              criteria = CR_OS_ARCH;
              condition = cJSON_GetStringValue(osArchItem);
            }
        }
      else
        {
          criteria = CR_ALWAYS;
        }
    }
}

Artifact::Artifact(const cJSON *src)
{
  if(!cJSON_IsObject(src))
    {
      return;
    }
  cJSON *pathItem = cJSON_GetObjectItem(src, "path");
  if(cJSON_IsString(pathItem))
    {
      path = cJSON_GetStringValue(pathItem);
    }
  else
    {
      pathItem = cJSON_GetObjectItem(src, "id");
      if(cJSON_IsString(pathItem))
        {
          path = cJSON_GetStringValue(pathItem);
        }
    }

  cJSON *sha1Item = cJSON_GetObjectItem(src, "sha1");
  if(cJSON_IsString(sha1Item))
    {
      sha1 = cJSON_GetStringValue(sha1Item);
    }

  cJSON *sizeItem = cJSON_GetObjectItem(src, "size");
  if(cJSON_IsNumber(sha1Item))
    {
      size = (size_t)cJSON_GetNumberValue(sizeItem);
    }

  cJSON *urlItem = cJSON_GetObjectItem(src, "url");
  if(cJSON_IsString(urlItem))
    {
      url = cJSON_GetStringValue(urlItem);
    }
}

Artifact::Artifact() {}

Library::Library(const cJSON *src)
{
  if(!cJSON_IsObject(src))
    {
      return;
    }

  cJSON *nameItem = cJSON_GetObjectItem(src, "name");
  if(cJSON_IsString(nameItem))
    {
      name = cJSON_GetStringValue(nameItem);
      auto parts = Commons::splitStr(name, ":");
      isNative = parts.size() == 4; // If it has 4 parts
    }

  cJSON *downloadsItem = cJSON_GetObjectItem(src, "downloads");
  if(cJSON_IsObject(downloadsItem))
    {
      cJSON *artifactItem = cJSON_GetObjectItem(downloadsItem, "artifact");
      if(cJSON_IsObject(artifactItem))
        {
          artifact = Artifact(artifactItem);
        }
    }

  cJSON *rulesItem = cJSON_GetObjectItem(src, "rules");
  if(cJSON_IsArray(rulesItem))
    {
      parseList(rules, rulesItem);
    }
}

Argument::Argument(const cJSON *src)
{
  if(src == NULL)
    {
      return;
    }
  if(cJSON_IsObject(src))
    {
      cJSON *valueItem = cJSON_GetObjectItem(src, "value");
      if(cJSON_IsArray(valueItem))
        {
          int valueItemSize = cJSON_GetArraySize(valueItem);
          for(int i = 0; i < valueItemSize; i++)
            {
              cJSON *itemCurrent = cJSON_GetArrayItem(valueItem, i);
              if(cJSON_IsString(itemCurrent))
                {
                  values.push_back(cJSON_GetStringValue(itemCurrent));
                }
            }
        }
      else if(cJSON_IsString(valueItem))
        {
          values.push_back(cJSON_GetStringValue(valueItem));
        }

      cJSON *rulesItem = cJSON_GetObjectItem(src, "rules");
      if(cJSON_IsArray(rulesItem))
        {
          parseList(rules, rulesItem);
        }
    }
  else if(cJSON_IsString(src))
    {
      values.push_back(cJSON_GetStringValue(src));
    }
}

VersionProfile::VersionProfile(const cJSON *src)
{
  cJSON *idItem = cJSON_GetObjectItem(src, "id");
  if(cJSON_IsString(idItem))
    {
      id = cJSON_GetStringValue(idItem);
    }

  cJSON *argsItem = cJSON_GetObjectItem(src, "arguments");
  if(cJSON_IsObject(argsItem))
    {
      cJSON *gameArgsItem = cJSON_GetObjectItem(argsItem, "game");
      parseList(gameArgs, gameArgsItem);
      cJSON *jvmArgsItem = cJSON_GetObjectItem(argsItem, "jvm");
      parseList(jvmArgs, jvmArgsItem);
    }
  cJSON *assetIndexItem = cJSON_GetObjectItem(src, "assetIndex");
  assetIndexArtifact = Artifact(assetIndexItem);

  cJSON *downloadsItem = cJSON_GetObjectItem(src, "downloads");
  if(cJSON_IsObject(downloadsItem))
    {
      cJSON *clientItem = cJSON_GetObjectItem(downloadsItem, "client");
      cJSON *clientMappingsItem
          = cJSON_GetObjectItem(downloadsItem, "client_mappings");
      clientArtifact = Artifact(clientItem);
      clientMappingsArtifact = Artifact(clientMappingsItem);

      // There are no path specified, so we better assign it
      clientArtifact.path = id + "/" + id + ".jar";
      clientMappingsArtifact.path = id + "/" + id + ".mappings";
    }

  cJSON *javaVersionStructItem = cJSON_GetObjectItem(src, "javaVersion");
  if(cJSON_IsObject(javaVersionStructItem))
    {
      cJSON *javaVersionNumberItem
          = cJSON_GetObjectItem(javaVersionStructItem, "majorVersion");
      if(cJSON_IsNumber(javaVersionNumberItem))
        {
          javaVersion
              = (unsigned int)cJSON_GetNumberValue(javaVersionNumberItem);
        }
    }

  cJSON *librariesItem = cJSON_GetObjectItem(src, "libraries");
  parseList(libraries, librariesItem);

  cJSON *logItem = cJSON_GetObjectItem(src, "logging");
  if(cJSON_IsObject(logItem))
    {
      cJSON *logClientItem = cJSON_GetObjectItem(logItem, "client");
      if(cJSON_IsObject(logClientItem))
        {
          cJSON *logArgItem = cJSON_GetObjectItem(logClientItem, "argument");
          if(cJSON_IsString(logArgItem))
            {
              logArgument = cJSON_GetStringValue(logArgItem);
            }

          cJSON *logFileItem = cJSON_GetObjectItem(logClientItem, "file");
          logConfigFileArtifact = Artifact(logFileItem);
        }
    }

  cJSON *mainClassItem = cJSON_GetObjectItem(src, "mainClass");
  if(cJSON_IsString(mainClassItem))
    {
      mainClass = cJSON_GetStringValue(mainClassItem);
    }

  cJSON *releaseTimeItem = cJSON_GetObjectItem(src, "releaseTime");
  if(cJSON_IsString(releaseTimeItem))
    {
      releaseTime = cJSON_GetStringValue(releaseTimeItem);
    }

  cJSON *installTimeItem = cJSON_GetObjectItem(src, "time");
  if(cJSON_IsString(installTimeItem))
    {
      installTime = cJSON_GetStringValue(installTimeItem);
    }

  cJSON *releaseTypeItem = cJSON_GetObjectItem(src, "type");
  if(cJSON_IsString(releaseTypeItem))
    {
      std::string rel = cJSON_GetStringValue(releaseTypeItem);
      if(rel == "release")
        {
          type = RT_RELEASE;
        }
      if(rel == "snapshot")
        {
          type = RT_SNAPSHOT;
        }
      if(rel == "old_alpha")
        {
          type = RT_ALPHA;
        }
      if(rel == "old_beta")
        {
          type = RT_BETA;
        }
    }
}

std::list<Asset>
loadAssetIndex(const std::string &src)
{
  std::list<Asset> out;
  cJSON *dat = cJSON_Parse(src.c_str());
  if(!cJSON_IsObject(dat))
    {
      cJSON_Delete(dat);
      return out;
    }
  cJSON *objects = cJSON_GetObjectItem(dat, "objects");
  if(!cJSON_IsObject(objects))
    {
      cJSON_Delete(dat);
      return out;
    }
  cJSON *currentChild = objects->child;
  while(cJSON_IsObject(currentChild))
    {
      cJSON *hash = cJSON_GetObjectItem(currentChild, "hash");
      cJSON *size = cJSON_GetObjectItem(currentChild, "size");
      if(cJSON_IsString(hash) && cJSON_IsNumber(size))
        {
          Asset a;
          a.hash = cJSON_GetStringValue(hash);
          a.size = (size_t)cJSON_GetNumberValue(size);
          out.push_back(a);
        }
      currentChild = currentChild->next;
    }
  cJSON_Delete(dat);
  return out;
}
}

}