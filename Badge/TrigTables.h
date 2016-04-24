#ifndef SineCosineLUT_H
#define SineCosineLUT_H

	float sineTable[256] = {
		0.000000, 0.024529, 0.049043, 0.073527, 0.097968, 0.122349, 0.146657, 0.170876, 0.194993, 0.218992, 0.242859, 0.266581, 0.290142, 0.313528, 0.336726, 0.359721, 0.382500, 0.405048, 0.427353, 0.449400, 0.471177, 0.492671, 0.513868, 0.534756, 0.555322, 0.575554, 0.595439, 0.614967, 0.634124, 0.652900, 0.671282, 0.689261, 0.706825, 0.723964, 0.740667, 0.756924, 0.772726, 0.788063, 0.802926, 0.817305, 0.831193, 0.844581, 0.857460, 0.869823, 0.881663, 0.892972, 0.903744, 0.913973, 0.923651, 0.932773, 0.941334, 0.949329, 0.956752, 0.963600, 0.969868, 0.975552, 0.980649, 0.985156, 0.989070, 0.992389, 0.995111, 0.997234, 0.998757, 0.999679, 1.000000, 0.999718, 0.998835, 0.997351, 0.995267, 0.992584, 0.989304, 0.985428, 0.980960, 0.975901, 0.970255, 0.964025, 0.957214, 0.949828, 0.941871, 0.933346, 0.924260, 0.914618, 0.904425, 0.893688, 0.882413, 0.870608, 0.858278, 0.845432, 0.832077, 0.818222, 0.803874, 0.789042, 0.773736, 0.757964, 0.741736, 0.725062, 0.707951, 0.690414, 0.672462, 0.654105, 0.635355, 0.616222, 0.596718, 0.576855, 0.556646, 0.536101, 0.515234, 0.494056, 0.472581, 0.450822, 0.428792, 0.406504, 0.383970, 0.361206, 0.338225, 0.315040, 0.291665, 0.268115, 0.244404, 0.220546, 0.196554, 0.172445, 0.148232, 0.123929, 0.099552, 0.075116, 0.050633, 0.026121, 0.001593, -0.022937, -0.047452, -0.071939, -0.096383, -0.120768, -0.145081, -0.169307, -0.193430, -0.217438, -0.241314, -0.265046, -0.288617, -0.312016, -0.335226, -0.358235, -0.381027, -0.403591, -0.425912, -0.447977, -0.469772, -0.491284, -0.512501, -0.533409, -0.553997, -0.574251, -0.594159, -0.613710, -0.632892, -0.651692, -0.670101, -0.688106, -0.705698, -0.722864, -0.739596, -0.755883, -0.771714, -0.787082, -0.801975, -0.816387, -0.830307, -0.843727, -0.856639, -0.869036, -0.880910, -0.892255, -0.903062, -0.913325, -0.923039, -0.932198, -0.940796, -0.948827, -0.956288, -0.963173, -0.969479, -0.975201, -0.980336, -0.984882, -0.988834, -0.992192, -0.994953, -0.997115, -0.998677, -0.999638, -0.999997, -0.999755, -0.998911, -0.997466, -0.995421, -0.992777, -0.989535, -0.985698, -0.981268, -0.976247, -0.970639, -0.964447, -0.957674, -0.950325, -0.942404, -0.933917, -0.924867, -0.915260, -0.905103, -0.894401, -0.883162, -0.871390, -0.859094, -0.846282, -0.832960, -0.819137, -0.804820, -0.790020, -0.774744, -0.759002, -0.742803, -0.726157, -0.709075, -0.691565, -0.673640, -0.655309, -0.636583, -0.617475, -0.597995, -0.578156, -0.557968, -0.537445, -0.516598, -0.495440, -0.473984, -0.452243, -0.430230, -0.407958, -0.385440, -0.362691, -0.339723, -0.316551, -0.293188, -0.269649, -0.245948, -0.222099, -0.198116, -0.174013, -0.149807, -0.125510, -0.101137, -0.076703, -0.052224, -0.003185 };

	float cosineTable[256] = {
		1.000000, 0.999699, 0.998797, 0.997293, 0.995190, 0.992487, 0.989187, 0.985293, 0.980805, 0.975727, 0.970061, 0.963813, 0.956984, 0.949579, 0.941603, 0.933060, 0.923956, 0.914295, 0.904085, 0.893331, 0.882039, 0.870216, 0.857869, 0.845007, 0.831635, 0.817764, 0.803400, 0.788553, 0.773231, 0.757444, 0.741202, 0.724513, 0.707388, 0.689838, 0.671872, 0.653503, 0.634739, 0.615594, 0.596079, 0.576205, 0.555984, 0.535429, 0.514551, 0.493364, 0.471879, 0.450111, 0.428072, 0.405776, 0.383235, 0.360464, 0.337476, 0.314284, 0.290904, 0.267348, 0.243632, 0.219769, 0.195774, 0.171661, 0.147444, 0.123139, 0.098760, 0.074321, 0.049838, 0.025325, 0.000796, -0.023733, -0.048247, -0.072733, -0.097175, -0.121559, -0.145869, -0.170091, -0.194212, -0.218215, -0.242087, -0.265813, -0.289380, -0.312772, -0.335976, -0.358978, -0.381764, -0.404320, -0.426633, -0.448689, -0.470475, -0.491978, -0.513185, -0.534083, -0.554660, -0.574903, -0.594800, -0.614339, -0.633508, -0.652296, -0.670692, -0.688684, -0.706262, -0.723414, -0.740132, -0.756404, -0.772221, -0.787573, -0.802451, -0.816846, -0.830750, -0.844154, -0.857050, -0.869430, -0.881287, -0.892614, -0.903403, -0.913649, -0.923345, -0.932486, -0.941065, -0.949078, -0.956520, -0.963387, -0.969673, -0.975377, -0.980493, -0.985019, -0.988953, -0.992291, -0.995032, -0.997175, -0.998717, -0.999659, -0.999999, -0.999737, -0.998874, -0.997409, -0.995344, -0.992681, -0.989420, -0.985563, -0.981114, -0.976074, -0.970447, -0.964236, -0.957445, -0.950077, -0.942138, -0.933632, -0.924564, -0.914939, -0.904764, -0.894045, -0.882788, -0.870999, -0.858687, -0.845857, -0.832519, -0.818679, -0.804347, -0.789531, -0.774240, -0.758483, -0.742270, -0.725610, -0.708513, -0.690990, -0.673051, -0.654707, -0.635969, -0.616849, -0.597357, -0.577506, -0.557307, -0.536773, -0.515916, -0.494748, -0.473283, -0.451533, -0.429511, -0.407231, -0.384706, -0.361949, -0.338974, -0.315796, -0.292427, -0.268882, -0.245176, -0.221322, -0.197335, -0.173229, -0.149019, -0.124720, -0.100345, -0.075909, -0.051428, -0.026917, -0.002389, 0.022141, 0.046657, 0.071145, 0.095590, 0.119978, 0.144293, 0.168522, 0.192649, 0.216660, 0.240542, 0.264278, 0.287855, 0.311259, 0.334476, 0.357491, 0.380291, 0.402863, 0.425192, 0.447265, 0.469069, 0.490591, 0.511817, 0.532736, 0.553334, 0.573599, 0.593519, 0.613081, 0.632275, 0.651088, 0.669510, 0.687529, 0.705133, 0.722314, 0.739060, 0.755361, 0.771208, 0.786590, 0.801500, 0.815927, 0.829863, 0.843299, 0.856228, 0.868642, 0.880533, 0.891895, 0.902719, 0.913001, 0.922733, 0.931910, 0.940525, 0.948575, 0.956055, 0.962959, 0.969283, 0.975024, 0.980179, 0.984743, 0.988715, 0.992092, 0.994873, 0.997054, 0.998635, 0.999995 };

#endif