void UTouch::touch_WriteData(byte data)
{
	byte temp;

	temp=data;
	cbi(P_CLK, B_CLK);

	for(byte count=0; count<8; count++)
	{
		if(temp & 0x80)
			sbi(P_DIN, B_DIN);
		else
			cbi(P_DIN, B_DIN);
		temp = temp << 1; 
		cbi(P_CLK, B_CLK);                
		sbi(P_CLK, B_CLK);
	}
}

word UTouch::touch_ReadData()
{
	word data = 0;

	for(byte count=0; count<12; count++)
	{
		data <<= 1;
		sbi(P_CLK, B_CLK);
		cbi(P_CLK, B_CLK); 
		delayMicroseconds(4); //// added to get more stable readings???                      
		if (rbi(P_DOUT, B_DOUT))
			data++;
	}
	return(data);
}

