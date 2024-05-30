### Trainer Class Switch Chance
_Written for pokeemerald-expansion 1.8.X_

`HasBadOdds` is a function that, in general terms, prompts mid-battle switches when the AI does not have a good matchup and has a good switchin candidate in their party. This function has always had a hardcoded 50% chance to return FALSE to avoid excessive switching that can bog down gameplay.

I thought it would be fun to tie this switching chance to Trainer Class, so Jugglers could switch constantly if appropriate, Gym Leaders could switch more than the average trainer, and Youngsters could barely switch at all, with all of those switches still being sensible ones based on the `HasBadOdds` check.

All of the trainer classes have been set to what was the hardcoded 50% chance by default. There's an example comment you can follow if you search for SAMPLE TRAINER_CLASS to modify the chances as you see fit! Any trainer classes without a specified switch chance will just use the default.
