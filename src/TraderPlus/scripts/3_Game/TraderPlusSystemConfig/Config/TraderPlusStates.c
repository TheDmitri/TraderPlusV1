class TraderPlusStates
{
  bool AcceptWorn, AcceptDamaged, AcceptBadlyDamaged;
  float CoefficientWorn = 0.75, CoefficientDamaged = 0.5, CoefficientBadlyDamaged = 0.25;

  void TraderPlusStates(bool aw, bool ad, bool abd)
  {
    AcceptWorn = aw;
    AcceptDamaged = ad;
    AcceptBadlyDamaged = abd;
  }
};
