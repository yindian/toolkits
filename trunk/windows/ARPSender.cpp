//////////////////////////////////////////////////////////////////////////////// 
// 
// ARPSender 
// 
// File : ARPSender.cpp 
// Comment : A program for sending ARP packet 
// 
// 
//////////////////////////////////////////////////////////////////////////////// 


#include <stdio.h> 
#include <Packet32.h>

#pragma comment (lib, "ws2_32.lib") 
#pragma comment (lib, "packet.lib") 

#define EPT_IP 0x0800 /* type: IP */ 
#define EPT_ARP 0x0806 /* type: ARP */ 
#define EPT_RARP 0x8035 /* type: RARP */ 
#define ARP_HARDWARE 0x0001 /* Dummy type for 802.3 frames */ 
#define ARP_REQUEST 0x0001 /* ARP request */ 
#define ARP_REPLY 0x0002 /* ARP reply */ 

#define Max_Num_Adapter 10 

bool GetMacAddr(char* szMacAddr_String, char* MacAddr); 

#pragma pack(push, 1) 

typedef struct ehhdr 
{ 
	unsigned char eh_dst[6]; /* destination ethernet addrress */ 
	unsigned char eh_src[6]; /* source ethernet addresss */ 
	unsigned short eh_type; /* ethernet pachet type */ 
}EHHDR, *PEHHDR; 


typedef struct arphdr 
{ 
	unsigned short arp_hrd; /* format of hardware address */ 
	unsigned short arp_pro; /* format of protocol address */ 
	unsigned char arp_hln; /* length of hardware address */ 
	unsigned char arp_pln; /* length of protocol address */ 
	unsigned short arp_op; /* ARP/RARP operation */ 

	unsigned char arp_sha[6]; /* sender hardware address */ 
	unsigned long arp_spa; /* sender protocol address */ 
	unsigned char arp_tha[6]; /* target hardware address */ 
	unsigned long arp_tpa; /* target protocol address */ 
}ARPHDR, *PARPHDR; 

typedef struct arpPacket 
{ 
	EHHDR ehhdr; 
	ARPHDR arphdr; 
} ARPPACKET, *PARPPACKET; 

#pragma pack(pop) 


void Usage() 
{ 
	printf("******************************************\n"); 
	printf("ARPSender\n"); 
	printf("\n"); 
	printf("Usage: ARPSender.exe sha spa tha tpa arp_op times\n"); 
	printf("\nComment:\n"); 
	printf("\tsha:the MAC address of packet sender, eg:AAAAAABBBBBB\n"); 
	printf("\tspa:the IP address of packet sender, eg:192.168.1.1\n"); 
	printf("\ttha:the MAC address of target\n"); 
	printf("\ttpa:the IP address of target\n"); 
	printf("\tarp_op: the operation of ARP, 1:request, 2:reply\n"); 
	printf("\ttimes: the times of sending ARP packet.(int)\n"); 
	printf("*******************************************\n"); 
} 


int ARPSender(int argc, char* argv[]) 
{ 
	static char AdapterList[Max_Num_Adapter][1024]; 
	char szPacketBuf[600]; 
	char MacAddr[6]; 

	LPADAPTER lpAdapter; 
	LPPACKET lpPacket; 
	CHAR AdapterName[2048]; 
	CHAR *temp,*temp1; 
	ARPPACKET ARPPacket; 

	ULONG AdapterLength = 1024; 

	int AdapterNum = 0; 
	int nRetCode, i; 
	int nARPOP = 0; 
	int nTimes = 0; 
	int nAdapter = 0; 

	Usage(); 
	if (argc < 7) 
	{ 
		return 0; 
	} 

	nARPOP = atoi(argv[5]); 
	if (!(nARPOP == 1 || nARPOP == 2)) 
	{ 
		printf("\nPlease enter the ARP op!\n"); 
	} 

	nTimes = atoi(argv[6]); 
	if (nTimes <= 0) 
	{ 
		nTimes = 1; 
	} 

	//Get The list of Adapter 
	if(PacketGetAdapterNames((char*)AdapterName, &AdapterLength) == FALSE) 
	{ 
		printf("Unable to retrieve the list of the adapters!\n"); 
		return 0; 
	} 
	else {
		printf(AdapterName);
	}
	temp = AdapterName; 
	temp1=AdapterName; 
	i = 0; 
	while ((*temp != '\0')||(*(temp-1) != '\0')) 
	{ 
		if (*temp == '\0') 
		{ 
			memcpy(AdapterList[i],temp1,(temp-temp1)*2); 
			temp1=temp+1; 
			i++; 
		} 

		temp++; 
	} 

	AdapterNum = i; 
	for (i = 0; i < AdapterNum; i++) 
	{ 
		printf("\n%d- %s\n", i+1, AdapterList[i]); 

	} 

	while((nAdapter <= 0) || (nAdapter > AdapterNum)) 
	{ 
		printf("\nPlease choose your Adapter:"); 
		scanf("%1d", &nAdapter); 
	} 

	printf("\n"); 

	//Default open the 0 
	lpAdapter = (LPADAPTER) PacketOpenAdapter((LPTSTR) AdapterList[nAdapter - 1]); 
	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE)) 
	{ 
		nRetCode = GetLastError(); 
		printf("Unable to open the driver, Error Code : %lx\n", nRetCode); 
		return 0; 
	} 

	lpPacket = PacketAllocatePacket(); 
	if(lpPacket == NULL) 
	{ 
		printf("\nError:failed to allocate the LPPACKET structure."); 
		return 0; 
	} 

	ZeroMemory(szPacketBuf, sizeof(szPacketBuf)); 

	if (!GetMacAddr(argv[3], MacAddr)) 
	{ 
		printf ("Get Mac address error!\n"); 
		return 0; 
	} 
	memcpy(ARPPacket.ehhdr.eh_dst, MacAddr, 6); 

	if (!GetMacAddr(argv[1], MacAddr)) 
	{ 
		printf ("Get Mac address error!\n"); 
		return 0; 
	} 
	memcpy(ARPPacket.ehhdr.eh_src, MacAddr, 6); 

	ARPPacket.ehhdr.eh_type = htons(EPT_ARP); 

	ARPPacket.arphdr.arp_hrd = htons(ARP_HARDWARE); 
	ARPPacket.arphdr.arp_pro = htons(EPT_IP); 
	ARPPacket.arphdr.arp_hln = 6; 
	ARPPacket.arphdr.arp_pln = 4; 
	ARPPacket.arphdr.arp_op = htons(nARPOP); 

	if (!GetMacAddr(argv[1], MacAddr)) 
	{ 
		printf ("Get Mac address error!\n"); 
		return 0; 
	} 
	memcpy(ARPPacket.arphdr.arp_sha, MacAddr, 6); 

	ARPPacket.arphdr.arp_spa = inet_addr(argv[2]); 

	if (!GetMacAddr(argv[3], MacAddr)) 
	{ 
		printf ("Get Mac address error!\n"); 
		return 0; 
	} 
	memcpy(ARPPacket.arphdr.arp_tha , MacAddr, 6); 

	ARPPacket.arphdr.arp_tpa = inet_addr(argv[4]); 

	memcpy(szPacketBuf, (char*)&ARPPacket, sizeof(ARPPacket)); 
	PacketInitPacket(lpPacket, szPacketBuf, 60); 

	if(PacketSetNumWrites(lpAdapter, 1)==FALSE) 
	{ 
		printf("warning: Unable to send more than one packet in a single write!\n"); 
	} 

	for (i = 1; i <= nTimes; i++) 
	{ 
		Sleep(10); 

		if(PacketSendPacket(lpAdapter, lpPacket, TRUE)==FALSE) 
		{ 
			printf("Error sending the packets!\n"); 
			return 0; 
		} 
		printf("."); 
	} 

	printf("\n"); 

	printf ("\nSend ok!\n"); 

	// close the adapter and exit 
	PacketFreePacket(lpPacket); 
	PacketCloseAdapter(lpAdapter); 

	return 0; 
}