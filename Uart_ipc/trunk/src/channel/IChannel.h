#ifndef ICHANNEL_H
#define ICHANNEL_H

class IChannel
{
public:
	IChannel(int channel) : m_channel(channel){}
	virtual ~IChannel(){}

protected:
	int m_channel;
};

#endif
