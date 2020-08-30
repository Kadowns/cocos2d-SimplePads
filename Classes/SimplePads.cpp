/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "SimplePads.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace ui;

const std::string LabelFont = "fonts/Roboto-Bold.ttf";

static bool LoopAudio = false;


Scene* SimplePads::createScene()
{
    return SimplePads::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SimplePads::init()
{
    if (!Scene::init())
    {
        return false;
    }


    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto winSize = Director::getInstance()->getVisibleSize();

    auto background = DrawNode::create();
    background->drawSolidRect(Vec2(0, 0), winSize, Color4F(0.3529f,0.5960f,0.8156f,1.0));
    this->addChild(background);

    std::vector<std::string> soundFiles1 = {
            "sounds/pad1_cowboy.wav",
            "sounds/pad2_cowboy.wav",
            "sounds/pad3_cowboy.wav",
            "sounds/pad4_cowboy.wav",
            "sounds/pad5_cowboy.wav",
            "sounds/pad6_cowboy.wav",
            "sounds/pad7_cowboy.wav",
            "sounds/pad8_cowboy.wav",
            "sounds/pad9_cowboy.wav",
            "sounds/pad10_cowboy.wav",
            "sounds/pad11_cowboy.wav",
            "sounds/pad12_cowboy.wav",
    };

    std::vector<std::string> soundFiles2 = {
            "sounds/pad13_cowboy.wav",
            "sounds/pad14_cowboy.wav",
            "sounds/pad15_cowboy.wav",
            "sounds/pad16_cowboy.wav",
            "sounds/pad17_cowboy.wav",
            "sounds/pad18_cowboy.wav",
            "sounds/pad19_cowboy.wav",
            "sounds/pad20_cowboy.wav",
            "sounds/pad21_cowboy.wav",
            "sounds/pad22_cowboy.wav",
            "sounds/pad23_cowboy.wav",
            "sounds/pad24_cowboy.wav",
    };
    for (auto& file : soundFiles1){
        SimpleAudioEngine::getInstance()->preloadEffect(file.c_str());
    }
    for (auto& file : soundFiles2){
        SimpleAudioEngine::getInstance()->preloadEffect(file.c_str());
    }


    this->addChild(createPadPanel("juce", Vec2(114.5f, 256.5f), soundFiles1, 3));
    this->addChild(createPadPanel("cricket", Vec2(winSize.width - 324.5f, 256.5f), soundFiles2, 3));

    auto instrumentsLabel = Label::createWithTTF("Instruments", LabelFont, 40);
    instrumentsLabel->setPosition(winSize.width / 2, (winSize.height / 2) + 9);
    this->addChild(instrumentsLabel);

    auto playButton = Button::create("button_play.png");
    playButton->setTitleFontName(LabelFont);
    playButton->setTitleFontSize(40);
    playButton->setTitleText("Play");
    playButton->setTitleColor(Color3B(0, 0, 0));
    playButton->setPosition(Vec2(winSize.width / 2, winSize.height - 308));
    this->addChild(playButton);

    auto title = Label::createWithTTF("Title", LabelFont, 40);
    title->setPosition(winSize.width / 2, winSize.height - 211);
    this->addChild(title);

    auto wavCheckBox = createCheckBox(Vec2(335, 1003), "WAV", "toggle_unselected.png", "toggle_selected.png");
    auto mp3CheckBox = createCheckBox(Vec2(335, 939), "MP3", "toggle_unselected.png", "toggle_selected.png");
    auto oggCheckBox = createCheckBox(Vec2(335, 875), "OGG", "toggle_unselected.png", "toggle_selected.png");

    wavCheckBox->setSelected(true);

    wavCheckBox->addEventListener([wavCheckBox, mp3CheckBox, oggCheckBox](Ref* sender, CheckBox::EventType type){
        wavCheckBox->setSelected(true);
        mp3CheckBox->setSelected(false);
        oggCheckBox->setSelected(false);
    });

    mp3CheckBox->addEventListener([wavCheckBox, mp3CheckBox, oggCheckBox](Ref* sender, CheckBox::EventType type){
        wavCheckBox->setSelected(false);
        mp3CheckBox->setSelected(true);
        oggCheckBox->setSelected(false);
    });

    oggCheckBox->addEventListener([wavCheckBox, mp3CheckBox, oggCheckBox](Ref* sender, CheckBox::EventType type){
        wavCheckBox->setSelected(false);
        mp3CheckBox->setSelected(false);
        oggCheckBox->setSelected(true);
    });

    this->addChild(wavCheckBox);
    this->addChild(mp3CheckBox);
    this->addChild(oggCheckBox);


    auto loopCheckBox = createCheckBox(Vec2(118, 802), "loop", "checkbox_background.png", "checkbox_mark.png");
    loopCheckBox->addEventListener([](Ref* sender, CheckBox::EventType type){
       switch(type){
           case CheckBox::EventType::SELECTED:
               LoopAudio = true;
               break;
           case CheckBox::EventType::UNSELECTED:
               LoopAudio = false;
               SimpleAudioEngine::getInstance()->stopAllEffects();
               break;
       }
    });
    loopCheckBox->setSelected(LoopAudio);
    this->addChild(loopCheckBox);

    return true;
}


void SimplePads::menuCloseCallback(Ref* pSender)
{
    SimpleAudioEngine::end();
    Director::getInstance()->end();
}

Button* SimplePads::createPad(const Vec2 &position, const std::string &soundFile) {
    auto button = Button::create("pad_normal.png", "pad_selected.png", "pad_disabled.png");

    button->setPosition(position);
    button->addTouchEventListener([button, soundFile](Ref* sender, Widget::TouchEventType type){
        switch(type){
            case Widget::TouchEventType::BEGAN:{
                auto scaleDown = ScaleTo::create(0.1f, 0.8f);
                button->runAction(scaleDown);
            }
            break;
            case Widget::TouchEventType::ENDED:{
                auto scaleUp = ScaleTo::create(0.1f, 1.3f);
                auto easeScaleUp = EaseElasticOut::create(scaleUp->clone());

                auto scaleDown = ScaleTo::create(0.5f, 1.0f);
                auto easeScaleDown = EaseOut::create(scaleDown->clone(), 3);

                button->runAction(Sequence::create(easeScaleUp, easeScaleDown, nullptr));
                SimpleAudioEngine::getInstance()->playEffect(soundFile.c_str(), LoopAudio);
            }
            break;
            case Widget::TouchEventType::CANCELED : {
                auto scaleDown = ScaleTo::create(0.1f, 1.0f);
                button->runAction(scaleDown);
            }
        }
    });
    return button;
}

cocos2d::Node * SimplePads::createPadPanel(const std::string &panelName, const Vec2 &position,
                                           const std::vector<std::string> &soundFiles, int columnCount) {

    auto panel = Node::create();
    panel->setPosition(position);

    float xOffset = 105.0f;
    float yOffset = 113.0f;

    float panelWidth = xOffset * (columnCount - 1);
    float nameHeight = yOffset * (soundFiles.size() / columnCount) - 35;


    auto name = Label::createWithTTF(panelName, LabelFont, 40);
    name->setPosition(Vec2(panelWidth / 2, nameHeight));

    panel->addChild(name);

    for (int i = 0; i < soundFiles.size(); i++){
        Vec2 padPosition = Vec2(xOffset * (i % columnCount), yOffset * (i / columnCount));
        auto pad = createPad(padPosition, soundFiles[i]);
        auto padSize = pad->getContentSize();
        panel->addChild(pad);
    }

    return panel;
}

cocos2d::ui::CheckBox *SimplePads::createCheckBox(const cocos2d::Vec2 &position, const std::string &name,
                                                  const std::string &unselectedImageFile,
                                                  const std::string &selectedImageFile) {
    auto checkbox = CheckBox::create(unselectedImageFile, selectedImageFile);
    checkbox->setPosition(position);

    auto nameLabel = Label::createWithTTF(name, LabelFont, 40);
    nameLabel->setPosition(Vec2(135, 15));

    checkbox->addChild(nameLabel);

    return checkbox;
}
