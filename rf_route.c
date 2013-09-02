// Filename:	rf_route.c
// Function:	rf·�ɲ����ݴ���
// Author:		wzd
// Date:			2013��8��27��22:43:24


#include "rf_route.h"


// �ж�·������Rid
INT8U RidSrcCheck(Rf_Route* routepacket)
{
	// �յ��������������ݣ�ֱ�ӷ���
	if( routepacket->Src.Sn_temp == g_module_id.Sn_temp )
	{
		//Log_printf("is me sent\r\n");
		return 0;
	}
	
	// g_rid��ͬ�Ž������ݴ��� ���� Դ��ַ��ͬ&& g_pre_mcuid.temp!= 
	if( g_pre_rid != routepacket->Rid || g_pre_src != routepacket->Src.Sn_temp )
	{
		//Log_printf("get data\r\n");
		g_pre_rid = routepacket->Rid;
		g_pre_src = routepacket->Src.Sn_temp;
		return 1;
	}
	else
	{
		//Log_printf("rid equal\r\n");	
		return 0;
	}
}

// �ж������Ƿ��Ƿ��͸���ģ��
INT8U IsMe(Rf_Route* routepacket)
{
//	INT16U test_id = 0,test_des = 0;
//	INT8U	temp_id_1,temp_id_2;
//	INT8U	temp_des_1,temp_des_2;
//	temp_id_1  = (g_module_id.Sn_temp&0x8FFF)>>8;
//	temp_id_2	 = (g_module_id.Sn_temp&0x8FFF);
//	
//	temp_des_1 = (routepacket->Des.Sn_temp&0x8FFF)>>8;
//	temp_des_2 = (routepacket->Des.Sn_temp&0x8FFF);
//	
//	Log_printf("Test_id:");
//	Usart_printf(&(temp_id_1),1);
//  Usart_printf(&(temp_id_2),1);
//  Log_printf("Test_des:");
//  Usart_printf(&(temp_des_1),1);
//  Usart_printf(&(temp_des_2),1);
  
  
  // �ж� Ŀ�ĵ�ַ ��  ģ���ַ �Ƿ���ͬ  �޳�·����Ϣ ���ֳ� ģ��ͻ�վ ģ�����λΪ0 ��վ���λΪ1
  // ��վid���λΪ1�����ַ������0x8xxx
  if( ( g_module_id.Sn_temp&0x8FFF ) == ( routepacket->Des.Sn_temp&0x8FFF ) )
  {
  	return 1;
	}
  else
    return 0;
}

//
// ���ݽṹ������ʾ��
// Pre Length Rid Key Gateway Route  protocoltype src des data
// ���������ǣ�Rid������������Ridֻ�������·�ɷ��ֵ�����
// ��վ����·�ɼ������ʱRid = 1��ʼ��ģ���routepacket.RfPreRid = 0��ʼ, ���ص��ж�Ӧ���ڽ��յ�RF���ݺ�У��ͼ�����ȷ
void AckARL(Rf_Route* routepacket,INT8U* psentrfdata)
{
	// ��ֱ�ӽ�ȡԭ�ṹ�����ݣ����ظ���վ
	INT8U i;
	INT8U checknum=0;
  INT8U routedatalength=0;
  INT8U routeprotocol=0;
  INT8U datalength=0;

  psentrfdata[0] = routepacket->Pre; 
  psentrfdata[1] = routepacket->Length; 
  psentrfdata[2] = routepacket->Rid;
  psentrfdata[3] = routepacket->Key;
  
  psentrfdata[4] = routepacket->Gateway.Sn[0];
  psentrfdata[5] = routepacket->Gateway.Sn[1];
  
  // ��·�����ݽ��д���
  routedatalength = ( (routepacket->RfRouteData.Orien & 0x0F)-1 )*2;
  psentrfdata[6] = (routepacket->RfRouteData.Orien&0x0F)|0x50;		// ���ķ���
  psentrfdata[7] = routepacket->RfRouteData.CRPL;
  // ��·�����Ϊ0ʱ��pRouteData��ָ������
  if( routedatalength != 0 )
  {
    //routepacket->RfRouteData.pRouteData = &psentrfdata[8];     // Ӧ�ý����ݷŵ������
    for(i=0;i<routedatalength;i++)
    	psentrfdata[8+i] = *(routepacket->RfRouteData.pRouteData+i);
  }
  // ������������ͺ���±�
  routeprotocol = routedatalength +2+6;                              // routelength+2+6 = ��һ�ֶ��±�
  psentrfdata[routeprotocol] = routepacket->ProtocolType; 
  // �ش�����ʱ��Src��Des����λ��
  psentrfdata[routeprotocol+1] = routepacket->Des.Sn[0];
  psentrfdata[routeprotocol+2] = routepacket->Des.Sn[1];
  psentrfdata[routeprotocol+3] = routepacket->Src.Sn[0];
  psentrfdata[routeprotocol+4] = routepacket->Src.Sn[1];
  // ���ݶγ���
  datalength = routepacket->Length - ( routedatalength+2 ) - 4 - 5;     // ���������ݳ���
  if( datalength != 0 )
  {
  	for( i=0;i<datalength;i++ )
  		psentrfdata[routeprotocol+5+i] = *(routepacket->pSensorData+i);
  }
  // routepacket->Length����û�а��� ���ֽ� У��� �����ֽڱ���
  // ����У���
//	for( i=0;i<routepacket->Length+3;i++ )  
//	{
//		psentrfdata[routeprotocol+5+datalength] += psentrfdata[i];
//	}
//	
	for( i=0;i<routepacket->Length+2;i++ )  
	{
		checknum += psentrfdata[i];
	}
	psentrfdata[routeprotocol+5+datalength] = checknum;
		
	// ��ȡ����ǰ·�����  ���ͻ��Ѳ����㻽���¼�  ���ڶ�CRPL�����˲���������������ķ���
	WorCarry[0] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2];
	WorCarry[1] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2+1];
	
	// ���ǵ�һ��·������뷢�ͻ��Ѳ�
	if( 1 != routepacket->RfRouteData.CRPL )
		CC1101_Wakeupcarry(WorCarry,2,2);
	
	halRfSendPacket(psentrfdata,routepacket->Length+3);
	g_rid++;
}
// ·��2����������
// AA 0B 01 81 80 01 51 01 01 00 02 80 01 8E
void test(Rf_Route* routepacket,INT8U* psentrfdata)
{
	// ��ֱ�ӽ�ȡԭ�ṹ�����ݣ����ظ���վ
	INT8U i;
	INT8U checknum=0;
  INT8U routedatalength=0;
  INT8U routeprotocol=0;
  INT8U datalength=0;


  psentrfdata[0] = routepacket->Pre; 												
  psentrfdata[1] = routepacket->Length; 
  psentrfdata[2] = routepacket->Rid;
  psentrfdata[3] = routepacket->Key;
  
  psentrfdata[4] = routepacket->Gateway.Sn[0];
  psentrfdata[5] = routepacket->Gateway.Sn[1];
  
  if( 0x81 == routepacket->Key )
  	routepacket->RfRouteData.Orien++;		// ����Ѱ·�ɴ������վʱ����1
	psentrfdata[6] = routepacket->RfRouteData.Orien;
	
	// key���λΪ1�����ն˵���վ CRPL++
	if ( routepacket->Key&0x80)
		routepacket->RfRouteData.CRPL++;
	else
		routepacket->RfRouteData.CRPL--;
			
  psentrfdata[7] = routepacket->RfRouteData.CRPL;

  // ��·�����ݽ��д���OrienΪ1 ʱ  CRPLΪ1
  //routedatalength = ( (routepacket->RfRouteData.Orien & 0x0F)-1 )*2;
  routedatalength = ( (routepacket->RfRouteData.Orien & 0x0F)-1 )*2;
  
  //Usart_printf(&routedatalength,1);
  
	if ( 0x81 == routepacket->Key ) 
	{
		if( (routepacket->RfRouteData.Orien & 0x0F) != 1 )
		{
			//Log_printf("  route  ");
			for (i=0; i<2; i++)
			{
				// (routepacket->RfRouteData.CRPL-1)*2-2   
				// (2-1)*2-2 = 0  (3-1)*2-2 = 2  (4-1)*2-2 =  4 (5-1)*2-2 = 6  (6-1)*2-2 = 8
				// ������ID����·�ɱ���
				psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2-2+i] = g_module_id.Sn[i];
				//Usart_printf(&psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2-2+i],1);
			}	
			//Log_printf("  ");
		}
	}
	else 
	{
	  if( routedatalength != 0 )
	  {
	    for(i=0;i<routedatalength;i++)
	    	psentrfdata[8+i] = *(routepacket->RfRouteData.pRouteData+i);
	  }
	}
	//Log_printf("  second ok  ");
	
	// ��������length
   //psentrfdata[1] = routepacket->Length = routepacket->Length+datalength+routedatalength;
   if( 0x81 == routepacket->Key )
			psentrfdata[1] = routepacket->Length = routepacket->Length+2;
			
  // ������������ͺ���±�
  routeprotocol = routedatalength +2+6;                              // routelength+2+6 = ��һ�ֶ��±�
  psentrfdata[routeprotocol] = routepacket->ProtocolType; 
  // Э�����ͺ����Ŀ�ĵ�ַ
  psentrfdata[routeprotocol+1] = routepacket->Src.Sn[0];
  psentrfdata[routeprotocol+2] = routepacket->Src.Sn[1];
  psentrfdata[routeprotocol+3] = routepacket->Des.Sn[0];
  psentrfdata[routeprotocol+4] = routepacket->Des.Sn[1]; 
  
  // ���ݶγ���
  datalength = routepacket->Length - ( routedatalength+2 ) - 4 - 5;     // ���������ݳ���
  if( datalength != 0 )
  {
  	for( i=0;i<datalength;i++ )
  		psentrfdata[routeprotocol+5+i] = *(routepacket->pSensorData+i);
  }
  // routepacket->Length����û�а��� ���ֽ� У��� �����ֽڱ���
  // ����У���
  Log_printf("    ");
	for( i=0;i<routepacket->Length+2;i++ )
	{
		checknum += psentrfdata[i];
		Usart_printf(psentrfdata+i,1);
	}
	Usart_printf(&checknum,1);
	Log_printf("    ");
	
	psentrfdata[routeprotocol+5+datalength] = checknum;
	
	// ���յ����ݺ�ͨ���㲥ת����ȥ
	if ( 0x81 == routepacket->Key ) 
	{
		// ���й㲥���� ��Ѱ·��
		WorCarry[0] = WorCarry[1] = 0xFF;	
	}
	else
	{
		// ת������ʱ��ֱ��ȡ·���е����ݽ��л����¼�
		WorCarry[0] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2];
		WorCarry[1] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2+1];
	}
	
	CC1101_Wakeupcarry(WorCarry,2,2);
	halRfSendPacket(psentrfdata,routepacket->Length+3);
}

// routesize Ϊ·�����ݳ���
void AssignRouteLevel(Rf_Route* routepacket,INT8U routesize)
{
  INT8U i;
  //INT8U eepromdata[20];
  INT8U *proutedata = routepacket->RfRouteData.pRouteData	;
  //if ( RidSrcCheck( routepacket ) )
  //{
		if ( IapCheckEEPROM(SEARCH_MODE,1) )
		{
			IapProgramByte(SEARCH_MODE,0x00);	
		}
	  g_getroute = IapReadByte(SEARCH_MODE);
	  g_search = 0x00;
	
	  // �޸�ģ��ID
	  if ( IapCheckEEPROM(MODEL_SN_ADDRESS,2) )
	  {
			IapProgramByte(MODEL_SN_ADDRESS,routepacket->Des.Sn[0]);
			IapProgramByte(MODEL_SN_ADDRESS+1,routepacket->Des.Sn[1]);  	
	  }
	  else
	  {
	  	IapEraseSector(MODEL_SN_ADDRESS);	
			IapProgramByte(MODEL_SN_ADDRESS,routepacket->Des.Sn[0]);
			IapProgramByte(MODEL_SN_ADDRESS+1,routepacket->Des.Sn[1]);
	  }
	
		if( routesize != 0 )
		{	
			if ( IapCheckEEPROM(ROUTEDATA_ADDRESS,512) )
			{
				// д��·���ֽڳ���
				IapProgramByte(ROUTEDATA_ADDRESS,routesize);
				// д��·������
				for(i=0;i<routesize;i++)
				{
					IapProgramByte(ROUTEDATA_ADDRESS+i+1,*(proutedata++));
				}
			}
			else
			{
				IapEraseSector(ROUTEDATA_ADDRESS);	
				// д��·���ֽڳ���
				IapProgramByte(ROUTEDATA_ADDRESS+2,routesize);
				// д��·������
				for(i=0;i<routesize;i++)
				{
					IapProgramByte(ROUTEDATA_ADDRESS+i+1,*(proutedata++));
				}			
			}
		}
		g_pre_src = g_module_id.Sn_temp;
		// 0x82		�ն���Ӧ��վ����·�ɼ�������
		routepacket->Key = 0x82;
		// �����ݴ洢��EEPROM�к󣬷�����Ӧ��Ϣ
		AckARL(routepacket,RfSentBuf);
  //}
}


void TransmitDataCommand(Rf_Route* routepacket)
{
	test(routepacket,RfSentBuf);
}

//void DirectTransmitDataCommand(Rf_Route* routepacket)
//{
//  // ��·�����Ϊ0ʱ��pRouteData��ָ������
//    //routepacket->RfRouteData.pRouteData = &psentrfdata[8];     // Ӧ�ý����ݷŵ������
//		// ��ģ��Id����·�ɱ���
////		if ( 0x81 == routepacket->Key )
////		{
////			//routepacket->RfRouteData.CRPL++;
////	    psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2] = g_module_id.Sn[0];
////	    psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2+1] = g_module_id.Sn[1];
////		}	 
//// 
////  if( routedatalength != 0 )
////  {
////    for(i=0;i<routedatalength;i++)
////    	psentrfdata[8+i] = *(routepacket->RfRouteData.pRouteData+i);
////  }
//	INT8U i;
//	INT8U checknum=0;
//  INT8U routedatalength=0;
//  INT8U routeprotocol=0;
//  INT8U datalength=0;
//
//  psentrfdata[0] = routepacket->Pre; 
//  psentrfdata[1] = routepacket->Length; 
//  psentrfdata[2] = routepacket->Rid;
//  psentrfdata[3] = routepacket->Key;
//  
//  psentrfdata[4] = routepacket->Gateway.Sn[0];
//  psentrfdata[5] = routepacket->Gateway.Sn[1];
//  
//  if( 0x81 == routepacket->Key )
//  	psentrfdata[6] = routepacket->RfRouteData.Orien+1;		// ����Ѱ·�ɴ������վʱ����1
//  else
//  	psentrfdata[6] = routepacket->RfRouteData.Orien;
//  
//  // ��·�����ݽ��д���OrienΪ0 ʱ  CRPLΪ1
//  routedatalength = ( (routepacket->RfRouteData.Orien & 0x0F)-1 )*2;
//	
//	// key���λΪ1�����ն˵���վ CRPL++
//	if ( routepacket->Key&0x80)
//		routepacket->RfRouteData.CRPL++;
//	else
//		routepacket->RfRouteData.CRPL--;
//			
//  psentrfdata[7] = routepacket->RfRouteData.CRPL;
//
//	if ( 0x81 == routepacket->Key ) 
//	{
//		if( (routepacket->RfRouteData.Orien & 0x0F) != 1 )
//		{
//			for (i=0; i<2; i++)
//			{
//				// (routepacket->RfRouteData.CRPL-1)*2-2   
//				// (2-1)*2-2 = 0  (3-1)*2-2 = 2  (4-1)*2-2 =  4 (5-1)*2-2 = 6  (6-1)*2-2 = 8
//				// ������ID����·�ɱ���
//				psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2-2+i] = g_module_id.Sn[i];
//			}	
//		}
//	}
//	else 
//	{
//	  if( routedatalength != 0 )
//	  {
//	    for(i=0;i<routedatalength;i++)
//	    	psentrfdata[8+i] = *(routepacket->RfRouteData.pRouteData+i);
//	  }
//	}
//  
//  // ������������ͺ���±�
//  routeprotocol = routedatalength +2+6;                              // routelength+2+6 = ��һ�ֶ��±�
//  psentrfdata[routeprotocol] = routepacket->ProtocolType; 
//  // ���ݶγ���
//  datalength = routepacket->Length - ( routedatalength+2 ) - 4 - 5;     // ���������ݳ���
//  if( datalength != 0 )
//  {
//  	for( i=0;i<datalength;i++ )
//  		psentrfdata[routeprotocol+5+i] = *(routepacket->pSensorData+i);
//  }
//  
//  psentrfdata[routeprotocol+1] = routepacket->Src.Sn[0];
//  psentrfdata[routeprotocol+2] = routepacket->Src.Sn[1];
//  psentrfdata[routeprotocol+3] = routepacket->Des.Sn[0];
//  psentrfdata[routeprotocol+4] = routepacket->Des.Sn[1];
//
//  // routepacket->Length����û�а��� ���ֽ� У��� �����ֽڱ���
//  // ����У���
//	for( i=0;i<routepacket->Length+3;i++ )
//	{
//		psentrfdata[routeprotocol+5+datalength] += psentrfdata[i];
//	}
//	
//	//WorCarry[0] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2];
//	//WorCarry[1] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2+1];	
//	// ���յ����ݺ�ͨ���㲥ת����ȥ
//	if ( 0x81 == routepacket->Key ) 
//	{
//		// ���й㲥���� ��Ѱ·��
//		WorCarry[0] = WorCarry[1] = 0xFF;	
//	}
//	else
//	{
//		// ת������ʱ��ֱ��ȡ·���е����ݽ��л����¼�
//		WorCarry[0] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2];
//		WorCarry[1] = psentrfdata[8+(routepacket->RfRouteData.CRPL-1)*2+1];
//	}
//	
//	CC1101_Wakeupcarry(WorCarry,2,2);
//	halRfSendPacket(psentrfdata,routepacket->Length+3);	
//}

//Bool RfCheckData(INT8U *prfdata,Rf_Route routepacket,INT8U *proutedata,INT8U *pSensorData)
void RfRouteManage(INT8U *prfdata,Rf_Route* routepacket)
{
  INT8U routedatalength = 0;
  INT8U routeprotocol = 0;
  INT8U datalength = 0;
  INT8U	checknum = 0;
  INT8U i;
  
  routepacket->Pre = prfdata[0]; 
  routepacket->Length = prfdata[1]; 
  routepacket->Rid = prfdata[2];
  routepacket->Key = prfdata[3]; 
  
  routepacket->Gateway.Sn[0] = prfdata[4];
  routepacket->Gateway.Sn[1] = prfdata[5];
  
  // ��·�����ݽ��д���  Orien ��1��ʼ 0101 xxxx Ϊ�ն˵���վ
  routedatalength = ( (routepacket->RfRouteData.Orien & 0x0F)-1)*2;
  routepacket->RfRouteData.Orien =  prfdata[6];
  routepacket->RfRouteData.CRPL =  prfdata[7];
  // ��·�����Ϊ0ʱ��pRouteData��ָ������
  if( routedatalength != 0 )
    routepacket->RfRouteData.pRouteData = &prfdata[8];     					 // Ӧ�ý����ݷŵ������
  // ������������ͺ���±�
  routeprotocol = routedatalength +2+6;                              // routelength+2+6 = ��һ�ֶ��±�
  routepacket->ProtocolType = prfdata[routeprotocol]; 
  // ģ��ID
  routepacket->Src.Sn[0] = prfdata[routeprotocol+1];
  routepacket->Src.Sn[1] = prfdata[routeprotocol+2];
  routepacket->Des.Sn[0] = prfdata[routeprotocol+3];
  routepacket->Des.Sn[1] = prfdata[routeprotocol+4];
  // ���ݶγ���
  datalength = routepacket->Length - ( routedatalength+2 ) - 4 - 5;  // ���������ݳ���
  if( datalength != 0 )
  	routepacket->pSensorData = &prfdata[routeprotocol+5];

  Log_printf("  RD    ");
  // У�����RF���պ���м��� ���������������ֵ�����֤ 
  //	routepacket->Length+3Ϊȫ������
 	for( i=0;i<(routepacket->Length+2);i++ )
	{
		// ����У���
		//psentrfdata[routeprotocol+5+datalength] += psentrfdata[i];
		checknum += prfdata[i];
		Usart_printf(prfdata+i,1);
	}
	Usart_printf(&checknum,1);
	Log_printf("    ");

	if(RidSrcCheck(routepacket))
	{
		Log_printf("  0001  ");
		if( IsMe(routepacket) )
		{
			Log_printf("  0002  ");
		  //Key ������Ϊ0x01ʱ Ϊ����·�ɼ������� ������Ϊ0x02ʱ��Ϊ������������
		  switch( routepacket->Key )    
		  { // ����·��Э��ʱ
		    case 0x01:
		    	AssignRouteLevel(routepacket,routedatalength);
		    	break;
		    case 0x03:
		    	AckARL(routepacket,RfSentBuf);
		    	break;
		    //case 0x81:  ACKARL(routepacket);
		    //case 0x82:  ACKTDC(routepacket);break;
		    default : 
		    	Log_printf("Not Route information\r\n");
		    	break;	
		  }
		}
		else
		{
			Log_printf("  0003  ");
//			if( 0x00 == g_getroute )
//			{
//				Log_printf("DirectTransmitDataCommand not ok\n");
//				//DirectTransmitDataCommand(routepacket);
//			}
//			else
				TransmitDataCommand(routepacket);
		}		
		
	}
	
}

// Key:
// 0x81		�ն��������·�ɼ�������
// 0x01		��վ��Ӧ�ն˷���·�ɼ�������
// 0x82		�ն���Ӧ��վ����·�ɼ�������

// 0x03		��վ������������
// 0x83		�ն���Ӧ������������			



//
//void AddRoute(Rf_Route routepacket)
//{
//	routepacket->RfRouteData.CRPL++;
//	AckARL(routepacket,RfSentBuf);	
//}
//
//void SubtractRoute(Rf_Route routepacket)
//{
//	routepacket->RfRouteData.CRPL--;
//	AckARL(routepacket,RfSentBuf);
//}


//void SwitchRoute(Rf_Route* routepacket)
//{
//	// ·�ɷ������ն˵���վ 
//	if (routepacket->Key&0x80)
//	{
//		switch
//		{
//			
//  		case 0x81:
//  			AddRoute(routepacket); 
//  		break;
//  		
//  		case 2:
//  			
//  		break;
//  		
//  		default:
//  		
//  		break;			
//			
//		}
//	}	
//	else // ��վ���ն�
//	{
//		
//	}
//	
//}





//	psentrfdata[0] = routepacket->Pre;
//	
//	psentrfdata[2] = g_rid;										// �����ȫ�ֱ���g_rid
//	psentrfdata[3] = 0x82;										// Key
//	psentrfdata[4] = routepacket->Gateway.Sn[0];	// Gateway
//	psentrfdata[5] = routepacket->Gateway.Sn[1];
//	
//	psentrfdata[6] = (routepacket->RfRouteData.Orien&0x0F)|0x50;	// ���Ĵ��䷽��
//	psentrfdata[7] = routepacket->RfRouteData.CRPL;





	//·�ɽڵ���ڵ���վ��·��������ֱ��ת������վ��
//	switch( routepacket->Key )    
//  { 
//  	// ת����վ����·�ɼ�������
//		case 0x01:  
//			//SwitchRoute(routepacket);
//			SubtractRoute(routepacket);
//			break;
//		
//		// 				
//		case 0x81:	
//			;
//			break;
//			
//    default : 
//    	Log_printf("Data not for me\n");
//    	break;	
//  }










//	psentrfdata[0] = routepacket->Pre;
//	
//	psentrfdata[2] = g_rid;										// �����ȫ�ֱ���g_rid
//	psentrfdata[3] = 0x82;										// Key
//	psentrfdata[4] = routepacket->Gateway.Sn[0];	// Gateway
//	psentrfdata[5] = routepacket->Gateway.Sn[1];
//	
//	psentrfdata[6] = (routepacket->RfRouteData.Orien&0x0F)|0x50;	// ���Ĵ��䷽��
//	psentrfdata[7] = routepacket->RfRouteData.CRPL;











