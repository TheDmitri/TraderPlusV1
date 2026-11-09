# Vanilla motorbikes: DayZ 1.30

Adds Motorbike_01_Blue/Red/Yellow and Motorbike_02_Blue/Green/Red/Yellow to the Garage trading and CarLock paths. Other vehicle classes keep their original code paths.

## Configuration and use

Enable UseGarageToTradeCar. Merge the seven example entries in docs/examples/motorbike-vehicle-parts.json into TraderPlusVehiclesConfig VehiclesParts, retaining the rest of the server configuration. Configure normal price entries separately. Purchase one motorcycle at a time without insurance. Use an original public Garage NPC and a clear parking point. Empty cargo and only supported vanilla attachments are accepted.

New purchases request full fuel. Parking/retrieval preserves the measured fuel amount. Restoration and bounded fuel retries run on subsequent server ticks; payment and removal from Garage occur only after verification. Failed/interrupted transactions may require administrative recovery, with the Garage entry preserved or quarantined.

A newly delivered motorcycle has an owner lock. The owner can use U to toggle it, or use the Lock/Unlock Motorbike hold actions after setting a PIN. Open Lock Menu sets the first eight-digit PIN or accepts an existing PIN. Installing the CarLock item manually opens that menu after the server confirms installation. Wrong supplied PINs are rejected, including on the owner's account; the owner alone can use the empty-PIN shortcut. Knowing a PIN does not transfer Garage ownership.

Pliers remove an unlocked owner's CarLock after the action completes, returning the device and applying tool wear. CarLockPick uses the configured LockPickChance, consumes the tool after a valid attempt and plays an alarm; the server also checks the safezone restriction. No new admin bypass is introduced. Bikes must be stationary, unoccupied and engine off for locking and parking operations.

## Persistence and limitations

Server-side records live in the Garage profile's TPMBOwners directory and $profile:TPMotorbikeCarLock. They are keyed by full native persistent IDs; lock snapshots use an opaque Garage vault reference. PINs remain in server profile files, never in Garage RPC snapshots or net-synced variables. Protect those profile files like other server administration data. No data is appended to native vehicle OnStoreSave/OnStoreLoad streams.

This path preserves fuel, condition and locks irrespective of the original SaveVehicle* flags. It does not provide cargo storage, insurance, resale, receipts, repair/refuel services, flag/private Garage support, bicycles or boats. It does not migrate private TLHBG predecessor entries. File staging/readback is not atomic with native CE persistence, so interrupted operations can require administrative reconciliation. Other mods' physical pushing/flipping/loading behaviors are outside this validation.

The original CarLock keyboard and mouse UI resources are reused. CARLOCKDISABLE guards exclude lock integration, but that compile configuration has not been separately runtime-tested.

## Validation evidence

The server owner confirmed standalone s01 tests with the private server addon disabled: purchase/delivery and fuel, preservation of partial fuel, mouse/U locking, wrong PIN rejection on the owner's account, restart persistence, pliers removal and CarLockPick with alarm.

Non-owner/second-account runtime access remains untested because no second player was available. The direct original-module overlay is proposed for maintainer review and has not been separately rebuilt/run; standalone runtime confirmation is not a claim of exhaustive regression coverage. Original car regressions and safezone/lockpick configuration combinations should be reviewed before release.

## Server-side checks and recovery

- The motorbike price, stock mode and accepted condition are read from the server's TraderPlusPriceConfig and stock files. Values sent by the client are ignored. The trader must list the category that contains the motorbike.
- If a purchase cannot be committed after payment, the money and the stock are returned and the staged Garage entry is removed.
- An interrupted motorbike entry (state Buying, Parking or Withdrawing) only blocks that motorbike. The rest of the player's Garage keeps working. The server log prints `[TPMB] Pending motorbike entry in Garage <id>` so an admin can set the entry back to `Stored` or remove it.
- A motorbike that was not loaded from storage (a new spawn or delivery) clears any owner or lock record left on disk for its persistent ID. This keeps stale records from a storage wipe off new bikes.
- Five wrong PINs on one motorbike, from any account, block its keypad for 10 minutes.

## Rolling back

Garage entries written by this version carry the extra fields TPMBVersion, TPMBState, TPMBToken, TPMBData and TPCMStateToken. A version without motorbike support ignores those fields and treats a stored motorbike as a normal vehicle entry. Before rolling back, have players take their motorbikes out of the Garage, or remove the Motorbike_* entries from TraderPlusGarageDatabase.
