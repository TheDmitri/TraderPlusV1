class TPMBPart
{
    string Type;
    int Slot;
    float Health;
    float Quantity;
    float Wet;
    float Energy;
};

class TPMBOwnerRecord
{
    string Owner;
    string Type;
    string Token;
};

class TPMBSnapshot
{
    ref array<ref TPMBPart> Parts = new array<ref TPMBPart>;
    ref TStringArray Zones = new TStringArray;
    ref TFloatArray ZoneHealth = new TFloatArray;
};

modded class VehiculeData
{
    // Existing car/boat entries keep defaults. No migration or schema version change.
    int TPMBVersion;
    string TPMBState;
    string TPMBToken;
    ref TPMBSnapshot TPMBData;
    string TPCMStateToken;
};
