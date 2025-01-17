/*
===============================================================================

 Copyright (C) 2023 Bernardo Escalona. All Rights Reserved.

  This file is part of AES70_OCP1_StringGenerator, found at:
  https://github.com/escalonely/AES70_OCP1_StringGenerator

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 3.0 as published
 by the Free Software Foundation.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

===============================================================================
*/

#include "TestPage.h"
#include "MainTabbedComponent.h"
#include "Common.h"


/**
 * Indeces of juce::Label components used on the GUI.
 */
enum GuiLabels
{
    LABELIDX_TITLE = 0,
    LABELIDX_IP_ADDRESS,
    LABELIDX_IP_PORT,
    LABELIDX_IP_STATUS,
    LABELIDX_MESSAGE_DISPLAY,
    LABELIDX_MAX,
};

/**
 * Text content for all juce::Label components used on the GUI.
 */
static const std::vector<juce::String> GuiLabelsText = {
    /* LABELIDX_TITLE              */ "AES70 OCP.1 binary string generator v" + juce::String(JUCE_STRINGIFY(JUCE_APP_VERSION)),
    /* LABELIDX_IP_ADDRESS,        */ "IP Address:",
    /* LABELIDX_IP_PORT,           */ "Port:",
    /* LABELIDX_IP_STATUS,         */ "Status:",
    /* LABELIDX_MESSAGE_DISPLAY,   */ "Inbox:",
    /* LABELIDX_MAX,               */ "MAX"
};


//==============================================================================
// Class TestPage
//==============================================================================

TestPage::TestPage(MainTabbedComponent* parent)
    :   AbstractPage(parent),
        m_ipAddressEdit(juce::TextEditor("IpAddressEdit")),
        m_ipPortEdit(juce::TextEditor("IpAddressEdit")),
        m_stateLed(juce::TextButton("StatusLed")),
        m_loadButton(juce::TextButton("Load config from file")),
        m_saveButton(juce::TextButton("Save config to file")),
        m_incomingMessageDisplayEdit(juce::TextEditor("MessageDisplayEdit")),
        m_hyperlink(juce::HyperlinkButton(ProjectHostShortURL, juce::URL(ProjectHostLongURL)))
{
    jassert(parent != nullptr);

    addAndMakeVisible(&m_hyperlink);
    addAndMakeVisible(&m_ipAddressEdit);
    addAndMakeVisible(&m_ipPortEdit);
    addAndMakeVisible(&m_stateLed);
    addAndMakeVisible(&m_incomingMessageDisplayEdit);
    addAndMakeVisible(&m_loadButton);
    addAndMakeVisible(&m_saveButton);

    // Create and add all labels on the GUI
    for (int labelIdx = 0; labelIdx < LABELIDX_MAX; labelIdx++)
    {
        m_ocaLabels.emplace_back(std::make_unique<juce::Label>("TestPage Label " + juce::String(labelIdx)));
        if (labelIdx == LABELIDX_TITLE)
            m_ocaLabels.back()->setJustificationType(juce::Justification::bottomRight);
        else
            m_ocaLabels.back()->setJustificationType(juce::Justification::centredRight);

        m_ocaLabels.back()->setColour(juce::Label::textColourId, LabelEnabledTextColour);
        m_ocaLabels.back()->setText(GuiLabelsText.at(labelIdx), juce::dontSendNotification);
        addAndMakeVisible(m_ocaLabels.back().get());
    }

    // Set same font as the labels.
    m_hyperlink.setFont(m_ocaLabels.at(LABELIDX_TITLE)->getFont(), false /* do not resize */);
    m_hyperlink.setJustificationType(juce::Justification::topRight);
    m_hyperlink.setTooltip(ProjectHostLongURL); // TODO: get tooltip to work

    m_ipAddressEdit.setHasFocusOutline(true);
    m_ipAddressEdit.setInputRestrictions(16, "0123456789.");
    m_ipAddressEdit.setIndents(m_ipAddressEdit.getLeftIndent(), 0); // Hack for JUCE justification bug
    m_ipAddressEdit.setJustification(juce::Justification(juce::Justification::centredRight));
    
    m_ipAddressEdit.onTextChange = [=]()
    {
        if (OnDeviceIpAddressChanged)
            OnDeviceIpAddressChanged(m_ipAddressEdit.getText(), m_ipPortEdit.getText().getIntValue());
    };

    m_ipPortEdit.setHasFocusOutline(true);
    m_ipPortEdit.setInputRestrictions(0, "0123456789");
    m_ipPortEdit.setIndents(m_ipAddressEdit.getLeftIndent(), 0); // Hack for JUCE justification bug
    m_ipPortEdit.setJustification(juce::Justification(juce::Justification::centredRight));

    m_ipPortEdit.onTextChange = [=]()
    {
        if (OnDeviceIpAddressChanged)
            OnDeviceIpAddressChanged(m_ipAddressEdit.getText(), m_ipPortEdit.getText().getIntValue());
    };

    // Initialize displayed ip address and port.
    juce::String address;
    int port;
    if (GetMainComponent()->GetConnectionParameters(address, port))
    {
        m_ipAddressEdit.setText(address, juce::dontSendNotification);
        m_ipPortEdit.setText(juce::String(port), juce::dontSendNotification);
    }

    m_stateLed.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    m_stateLed.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    m_stateLed.setToggleState(false, dontSendNotification);
    m_stateLed.setEnabled(false);

    m_incomingMessageDisplayEdit.setHasFocusOutline(true);
    m_incomingMessageDisplayEdit.setReadOnly(true);
    m_incomingMessageDisplayEdit.setCaretVisible(false);
    m_incomingMessageDisplayEdit.setMultiLine(true, true);
    m_incomingMessageDisplayEdit.setScrollbarsShown(true);
    m_incomingMessageDisplayEdit.setTextToShowWhenEmpty("This field will show incoming AES70 OCP.1 "
        "messages, should any arrive.",
        LabelEnabledTextColour);

    m_loadButton.setClickingTogglesState(false);
    m_loadButton.setColour(juce::TextButton::ColourIds::buttonColourId, ButtonBackgroundColour);
    m_loadButton.setColour(juce::TextButton::ColourIds::textColourOffId, LabelEnabledTextColour);
    m_loadButton.onClick = [=]()
        {
            GetMainComponent()->LoadFileViaDialog();
        };

    m_saveButton.setClickingTogglesState(false);
    m_saveButton.setColour(juce::TextButton::ColourIds::buttonColourId, ButtonBackgroundColour);
    m_saveButton.setColour(juce::TextButton::ColourIds::textColourOffId, LabelEnabledTextColour);
    m_saveButton.onClick = [=]()
        {
            GetMainComponent()->SaveFileViaDialog();
        };

    setSize(10, 10);
}

TestPage::~TestPage()
{

}

void TestPage::AddMessage(const juce::MemoryBlock& message)
{
    juce::String incomingString = juce::String::toHexString(message.getData(), static_cast<int>(message.getSize()));

    // TODO: make addition of timestamp optional
    String timestamp = Time::getCurrentTime().toString(true, true, true, true);
    incomingString = timestamp + juce::String(": ") + incomingString + juce::String("\r\n");

    m_incomingMessageDisplayEdit.moveCaretToEnd();
    m_incomingMessageDisplayEdit.insertTextAtCaret(incomingString);
}

void TestPage::UpdateConnectionStatus(ConnectionStatus status)
{
    juce::String statusString("?");

    switch (status)
    {
        case ConnectionStatus::Online:
            m_stateLed.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
            m_stateLed.setToggleState(true, juce::dontSendNotification);
            statusString = juce::String("Online");
            break;

        case ConnectionStatus::Offline:
        default:
            m_stateLed.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orangered);
            m_stateLed.setToggleState(true, juce::dontSendNotification);
            statusString = juce::String("Offline");
            break;
    }

    m_stateLed.setButtonText(statusString);
    DBG("SetConnectionStatus: " + statusString);
}

void TestPage::paint(juce::Graphics& g)
{
    // TODO: anything to do here? Just call base implementation for now.

    juce::Component::paint(g);
}

void TestPage::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromLeft(4); // Margins
    bounds.removeFromRight(4);
    int margin = 2;
    int controlHeight = 40;
    int comboBoxWidth = bounds.getWidth() / 7; // 7 columns. 

    // Row 1
    auto rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 0.75));
    rowBounds.removeFromLeft(comboBoxWidth); // Horizontal spacer
    m_ocaLabels.at(LABELIDX_TITLE)->setBounds(rowBounds.reduced(margin));
    rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 0.5));
    m_hyperlink.changeWidthToFitText();
    m_hyperlink.setBounds(rowBounds.removeFromRight(m_hyperlink.getWidth()).reduced(margin));

    // Vertical spacer
    bounds.removeFromTop(controlHeight / 2);

    // Row 2
    rowBounds = bounds.removeFromTop(controlHeight);
    rowBounds.removeFromLeft(comboBoxWidth); // Horizontal spacer
    m_saveButton.setBounds(rowBounds.removeFromRight(comboBoxWidth * 2).reduced(margin));
    m_loadButton.setBounds(rowBounds.removeFromRight(comboBoxWidth * 2).reduced(margin));

    // Vertical spacer
    bounds.removeFromTop(controlHeight / 2);

    // Row 3
    rowBounds = bounds.removeFromTop(controlHeight);
    m_ocaLabels.at(LABELIDX_IP_ADDRESS)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ipAddressEdit.setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));
    m_ocaLabels.at(LABELIDX_IP_PORT)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ipPortEdit.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaLabels.at(LABELIDX_IP_STATUS)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_stateLed.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Vertical spacer
    bounds.removeFromTop(controlHeight / 2);

    // Row 7
    rowBounds = bounds.removeFromTop(bounds.getHeight() - margin * 2);
    m_incomingMessageDisplayEdit.setBounds(rowBounds.reduced(margin));

    juce::Component::resized();
}

