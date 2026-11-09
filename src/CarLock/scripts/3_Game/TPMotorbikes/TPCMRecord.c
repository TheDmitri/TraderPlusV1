// Server-only contents. Never embed this record in a GarageData RPC payload.
class TPCMRecord
{
    int Version=1;
    string Type;
    string Owner;
    string Password;
    bool Locked;
};
