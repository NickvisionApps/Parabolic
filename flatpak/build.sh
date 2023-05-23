#!/bin/sh

if [ -z $1 ]
then
    echo "Usage: $0 UI"
    exit 1
fi

if [ -d "nuget-sources" ]
then
    mv flatpak/NuGet.config ./
    mv cake-sources/cake.tool.3.0.0.nupkg flatpak/cake_repo/cake.tool/3.0.0/
    mv cake-sources/cake.filehelpers.6.1.3.nupkg flatpak/cake_repo/cake.filehelpers/6.1.3/
    dotnet tool restore
    dotnet cake --target=Publish --prefix=/app --ui=$1 --self-contained --sources=nuget-sources
else
    dotnet tool restore
    dotnet cake --target=Publish --prefix=/app --ui=$1 --self-contained
fi

dotnet cake --target=Install
