xcrun xcodebuild \
  -scheme test_tinyunit_example \
  -project tinyunit.xcodeproj \
  -configuration RelWithDebInfo \
  -destination 'platform=iOS Simulator,name=iPhone 7,OS=10.1' \
  -derivedDataPath \
  test_tinyunit_example.xcrun

# Show Device list
xcrun simctl list
# Show template list
# xcrun instruments -s templates
xcrun instruments -w "iPhone SE (10.1)"

export CONFIGURATION=RelWithDebInfo
export IOS_DECICE_ID=0C9D97E6-734D-4662-8E7F-C09A5F088E45
open /Applications/Xcode.app/Contents/Developer/Applications/Simulator.app --args -CurrentDeviceUDID $IOS_DECICE_ID

export APP_BUNDLE=RelWithDebInfo-iphonesimulator/test_tinyunit_example.app
xcrun simctl install $IOS_DECICE_ID $APP_BUNDLE
xcrun simctl launch --console $IOS_DECICE_ID  com.example

xcrun simctl install booted RelWithDebInfo-iphonesimulator/test_tinyunit_example.app
http://stackoverflow.com/questions/26031601/xcode-6-launch-simulator-from-command-line
  ios-sim launch --devicetypeid "iPhone-5, 10.1" RelWithDebInfo-iphonesimulator/test_tinyunit_example.app
xcrun simctl launch booted