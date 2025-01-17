![TitleBar.png](Resources/Documentation/TitleBar.png "0x3b")

# AES70 OCP.1 PDU Generator - Video Tutorial Part 1: Quick Start

Welcome to the first video tutorial for the AES70 OCP.1 PDU Generator. 

This video will teach you the core functionality of the tool. Keep in mind that some prior knowledge about AES70 may be required.

## What is this tool for?

The AES70 OCP.1 PDU Generator is a companion and debugging tool for integrating AES70-capable devices into remote control solutions.

It generates binary strings or PDUs for a variety of common Commands defined by the AES70 standard. 

These PDUs are represented as text in hexadecimal notation. This text can then be copy-and-pasted into the control environment being used. A LUA or python script may be used to send these PDUs over a TCP connection, for example.

## Getting started

An AES70-capable device hosts a number of AES70 "objects" which can be accessible over the network. These objects represent the state of the device, and can be used as an interface to interact with it.

To start using this tool, we need to know exactly which objects we wish to interact with. 

### Class

Every AES70 object belongs to a specific class. The class of an object determines what functionality it offers. 
There are many classes defined by AES70. This tool currently only supports the most common ones, but more will be added in the future.

> It is important to know that the AES70 class structure is hierarchical. This means that every class is a specialization of a more generic "parent" class. 
> A sub-class inherits all the functionality and Properties from its parent class.

Let's say we want to use this tool to generate a Command to set the output channel gain setting on our AES70-capable amplifier. In this case we select OcaGain from the list.

### ONo

The next thing we need to know about the object we wish to interact with is its object number, or ONo for short.

Every object hosted by an AES70-capable device has a unique ONo that is used to identify that object. We can think of it as a kind of object ID.

> Keep in mind that a device can host several objects of the same class, but each instance of that class needs to have its own ONo.
> For example, a two-channel amplifier could have two OcaGain objects, one for channel A with the ONo 10000, and one for channel B with the ONo 10001. 

Aside from a few reserved numbers, it is up to the manufacturer to choose whatever ONos they want to use in their AES70-capable device. So if we want to integrate with a particular device we need to obtain the required ONos from the manufacturer.

### Property

Next we need to specify which Property we want to query or modify in the specified object.

Each AES70 class defines a number of so-called Properties that are used to represent a state or a setting in the physical device.

Additionally, each AES70 class inherits all the Properties defined by its parent classes.

Selecting a Property from the list will show us the level within the AES70 class hierarchy at which this Property was defined, as well as the data type used.

> For example, the OcaGain class defines the "Gain" Property used to represent a gain value in dB. But it also defines the "Enabled" Property which is inherited from OcaWorker, and the "Role" Property which is inherited from OcaRoot. Let's stay with Gain for now.

### SetValue Command

Each AES70 class also defines a number of Commands or methods, which can be invoked on objects of that class. The most common Commands are to get and to set the value of each Property. 

If we select SetValue, we can then specify the value which we want to set the selected Property to. Drag the slider to adjust the value, or click on the slider to type in an exact number.

As you change the value you can already see the binary string below changing aswell.

### Binary string

What we see in the first of the two large fields below is the result of our configuration above. It contains a textual hex-based representation of the TCP/IP message, which needs to be sent to the device to perform the desired Command.

> Note that the generated PDU needs to be sent as binary and not as text, as done in other protocols such as OSC.

The binary string or PDU will need to be copied from here into a script, a piece of code, or whatever integration environment is being used.

This tool does offer the possibility to immediately test the Command with a real connected device. But we will come back to this later. 

### Response

In addition to the Command PDU, this tool also generates a simulated Response PDU, shown on the lowermost textfield.

This is the PDU which a real AES70-capable device would be expected to send back to us as a Response to the above Command.

The Response for a SetValue Command contains really only one piece of interesting information: the Response Status. 

> For example: if an invalid or non-existing ONo was used in the Command, the device is expected to respond with a "Bad Ono" Status.
> This Status can be selected on the tool's UI in order to simulate such an error scenario.
> This reverse-engineering of Responses can help to debug Commands until the "OK" Status is obtained, meaning that the device was able to perform the Command successfully.

### GetValue Command and Response

If the GetValue Command is selected instead, the Command's PDU becomes very simple: please provide the Property's current value. 

The Response PDU on the other hand, is now expected to contain the requested value. 

The tool provides a UI element that can be used to set the value which the Response should simulate. 

In the case of gain we know that the value is of type Float32, which means it takes up four bytes. In the Response field you can see those four bytes changing as we drag the slider to adjust the value. 

This feature of the tool is meant to help decode actual Responses received from real AES70-capable devices.

> Note that the requested value will not be included in a Response if the Status is not OK.

## Testing Commands

Next to the Command PDU the tool features a Test button, which is initially disabled. To enable it we need to go into the Test tab. 

In this page we are able to set up a network connection with a real test device. 

Simply enter the device's IP address and the TCP port number that the device uses to listen to AES70 messages.

> AES70 defines a recommended range to be used by manufacturers as the listening port of their devices. 
> Information about the port used by the actual test device will need to be obtained from the device's manufacturer.

The tool will attempt to establish a connection and will display the connection state on the Test page at all times. 

Going back to the PDU generator page, we see that now that the state is Online, the Test button has become enabled.

Clicking on the Test button will actually send the configured Command to the remote device.

If the device at the other end is AES70-capable, it will respond to our Command. 

The number shown on the Test tab will indicate how many AES70 Responses have been received since the last time we visited the Test page. 

Every time the Test button is clicked, the Command is sent and a new Response is received.

Switching to the Test tab will reset the number of unread Responses. 

On the field at the bottom of the Test page, all received Responses will be displayed with their corresponding time stamps. 

Keep in mind that the Responses shown here were actually sent by the test device, and not simply generated or simulated by this tool.

Comparing the real and the expected Responses can help us debug our Commands and reverse-engineer the Responses.

This concludes the tutorial on the core functionality of the AES70 OCP.1 PDU Generator tool. 

More advanced features will be covered on a future video.

Thanks for watching, and until next time.
