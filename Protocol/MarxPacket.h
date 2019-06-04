struct MarxPacket
{
    char authSessionToken[16];
    char command[4];
    char flag[1];
};