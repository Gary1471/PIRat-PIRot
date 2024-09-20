# PIRat & PIRot

Cílem práce je vytvořit funkční prototyp zařízení PIRat a PIRot. PIRat (vysílač) detekuje pomocí PIR senzoru pohyb člověka/zvířete a pošle bezdrátový signál pomocí ESP-NOW zařízení PIRot (přijímač), který zahlásí upozornění o pohybu. PIRat bude mít dva módy, první „skrytý mód“ jen posílá upozornění na PIRot, druhý „normální mód“ pošle upozornění a zároveň sám PIRat upozorní zvukovým znamením. To umožňuje používat zařízení PIRat samostatně bez zařízení PIRot. PIRot bude mít též dva módy, první „skrytý mód“ upozorní uživatele světelným znamením, druhý „normální mód“ upozorní uživatele světelným i zvukovým znamením.
