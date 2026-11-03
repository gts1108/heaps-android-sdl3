# Android SDL3 template project for Heaps.io
A barebones Heaps.io app that Could Technically Work But I Can't Be Sure, based on SDL3 template Android project. Uses SDL3 and OpenAL.

## Installation
The app crashes on the latest version of SDL. You need to manually rebase SDL to version 3.2.20 like this:
```bash
git -C android/app/jni/SDL checkout release-3.2.20
```
Other than that, it should work out of the box.

To build the project, do this:
```bash
haxe android.hxml
cd ./android
./gradlew assembleDebug
```
If everything goes well, the built apk will appear in `android/app/build/outputs/apk/debug`. 

## Points of interest
`android/keystore.properties` - keystore properties for signing the release build
`android/app/build.gradle` (not to be confused with `android/build.gradle`) - gradle configuration, package name and other
`android/app/res/values/strings.xml` - to change the app name