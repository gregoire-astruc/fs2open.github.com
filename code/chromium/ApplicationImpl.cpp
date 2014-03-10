
#include "chromium/ApplicationImpl.h"
#include "cfile/cfile.h"

#include <boost/unordered_map.hpp>

#include "include/cef_url.h"
#include "include/wrapper/cef_stream_resource_handler.h"

#include "VFSPP/util.hpp"
#include <boost/thread/lock_guard.hpp>
#include <chromium/jsapi/jsapi.h>

namespace
{

	// This was generated from the Apache mime.types file. It also contains tbl and tbm as text/plain
	template<typename T>
	void fillMimeMap(T& map)
	{
		map.insert(std::make_pair("scm", "application/vnd.lotus-screencam"));
		map.insert(std::make_pair("mny", "application/x-msmoney"));
		map.insert(std::make_pair("dist", "application/octet-stream"));
		map.insert(std::make_pair("gv", "text/vnd.graphviz"));
		map.insert(std::make_pair("mpkg", "application/vnd.apple.installer+xml"));
		map.insert(std::make_pair("edm", "application/vnd.novadigm.edm"));
		map.insert(std::make_pair("otp", "application/vnd.oasis.opendocument.presentation-template"));
		map.insert(std::make_pair("mp4", "video/mp4"));
		map.insert(std::make_pair("woff", "application/x-font-woff"));
		map.insert(std::make_pair("mp3", "audio/mpeg"));
		map.insert(std::make_pair("edx", "application/vnd.novadigm.edx"));
		map.insert(std::make_pair("scq", "application/scvp-cv-request"));
		map.insert(std::make_pair("scs", "application/scvp-cv-response"));
		map.insert(std::make_pair("osf", "application/vnd.yamaha.openscoreformat"));
		map.insert(std::make_pair("wmz", "application/x-ms-wmz"));
		map.insert(std::make_pair("ots", "application/vnd.oasis.opendocument.spreadsheet-template"));
		map.insert(std::make_pair("zmm", "application/vnd.handheld-entertainment+xml"));
		map.insert(std::make_pair("ott", "application/vnd.oasis.opendocument.text-template"));
		map.insert(std::make_pair("tr", "text/troff"));
		map.insert(std::make_pair("wmv", "video/x-ms-wmv"));
		map.insert(std::make_pair("mwf", "application/vnd.mfer"));
		map.insert(std::make_pair("scd", "application/x-msschedule"));
		map.insert(std::make_pair("fh5", "image/x-freehand"));
		map.insert(std::make_pair("fh4", "image/x-freehand"));
		map.insert(std::make_pair("wma", "audio/x-ms-wma"));
		map.insert(std::make_pair("wmf", "application/x-msmetafile"));
		map.insert(std::make_pair("wmd", "application/x-ms-wmd"));
		map.insert(std::make_pair("vxml", "application/voicexml+xml"));
		map.insert(std::make_pair("mpe", "video/mpeg"));
		map.insert(std::make_pair("g3", "image/g3fax"));
		map.insert(std::make_pair("mpc", "application/vnd.mophun.certificate"));
		map.insert(std::make_pair("lrf", "application/octet-stream"));
		map.insert(std::make_pair("mpn", "application/vnd.mophun.application"));
		map.insert(std::make_pair("mpm", "application/vnd.blueice.multipass"));
		map.insert(std::make_pair("lrm", "application/vnd.ms-lrm"));
		map.insert(std::make_pair("rl", "application/resource-lists+xml"));
		map.insert(std::make_pair("mpt", "application/vnd.ms-project"));
		map.insert(std::make_pair("jisp", "application/vnd.jisp"));
		map.insert(std::make_pair("smf", "application/vnd.stardivision.math"));
		map.insert(std::make_pair("mpy", "application/vnd.ibm.minipay"));
		map.insert(std::make_pair("cml", "chemical/x-cml"));
		map.insert(std::make_pair("trm", "application/x-msterminal"));
		map.insert(std::make_pair("tra", "application/vnd.trueapp"));
		map.insert(std::make_pair("hbci", "application/vnd.hbci"));
		map.insert(std::make_pair("f", "text/x-fortran"));
		map.insert(std::make_pair("cmc", "application/vnd.cosmocaller"));
		map.insert(std::make_pair("dll", "application/x-msdownload"));
		map.insert(std::make_pair("fhc", "image/x-freehand"));
		map.insert(std::make_pair("thmx", "application/vnd.ms-officetheme"));
		map.insert(std::make_pair("pbd", "application/vnd.powerbuilder6"));
		map.insert(std::make_pair("aas", "application/x-authorware-seg"));
		map.insert(std::make_pair("gqf", "application/vnd.grafeq"));
		map.insert(std::make_pair("pbm", "image/x-portable-bitmap"));
		map.insert(std::make_pair("mp2", "audio/mpeg"));
		map.insert(std::make_pair("tmo", "application/vnd.tmobile-livetv"));
		map.insert(std::make_pair("dir", "application/x-director"));
		map.insert(std::make_pair("distz", "application/octet-stream"));
		map.insert(std::make_pair("tei", "application/tei+xml"));
		map.insert(std::make_pair("mcurl", "text/vnd.curl.mcurl"));
		map.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
		map.insert(std::make_pair("htm", "text/html"));
		map.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
		map.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
		map.insert(std::make_pair("tex", "application/x-tex"));
		map.insert(std::make_pair("odi", "application/vnd.oasis.opendocument.image"));
		map.insert(std::make_pair("odm", "application/vnd.oasis.opendocument.text-master"));
		map.insert(std::make_pair("pml", "application/vnd.ctc-posml"));
		map.insert(std::make_pair("oda", "application/oda"));
		map.insert(std::make_pair("odb", "application/vnd.oasis.opendocument.database"));
		map.insert(std::make_pair("odc", "application/vnd.oasis.opendocument.chart"));
		map.insert(std::make_pair("odf", "application/vnd.oasis.opendocument.formula"));
		map.insert(std::make_pair("odg", "application/vnd.oasis.opendocument.graphics"));
		map.insert(std::make_pair("bdf", "application/x-font-bdf"));
		map.insert(std::make_pair("sub", "image/vnd.dvb.subtitle"));
		map.insert(std::make_pair("xop", "application/xop+xml"));
		map.insert(std::make_pair("bdm", "application/vnd.syncml.dm+wbxml"));
		map.insert(std::make_pair("rtx", "text/richtext"));
		map.insert(std::make_pair("dcurl", "text/vnd.curl.dcurl"));
		map.insert(std::make_pair("sus", "application/vnd.sus-calendar"));
		map.insert(std::make_pair("u32", "application/x-authorware-bin"));
		map.insert(std::make_pair("abw", "application/x-abiword"));
		map.insert(std::make_pair("ggt", "application/vnd.geogebra.tool"));
		map.insert(std::make_pair("asx", "video/x-ms-asf"));
		map.insert(std::make_pair("ms", "text/troff"));
		map.insert(std::make_pair("sfd-hdstx", "application/vnd.hydrostatix.sof-data"));
		map.insert(std::make_pair("les", "application/vnd.hhe.lesson-player"));
		map.insert(std::make_pair("rlc", "image/vnd.fujixerox.edmics-rlc"));
		map.insert(std::make_pair("ggb", "application/vnd.geogebra.file"));
		map.insert(std::make_pair("mmr", "image/vnd.fujixerox.edmics-mmr"));
		map.insert(std::make_pair("pclxl", "application/vnd.hp-pclxl"));
		map.insert(std::make_pair("teacher", "application/vnd.smart.teacher"));
		map.insert(std::make_pair("box", "application/vnd.previewsystems.box"));
		map.insert(std::make_pair("g2w", "application/vnd.geoplan"));
		map.insert(std::make_pair("wdb", "application/vnd.ms-works"));
		map.insert(std::make_pair("onepkg", "application/onenote"));
		map.insert(std::make_pair("ahead", "application/vnd.ahead.space"));
		map.insert(std::make_pair("m3u", "audio/x-mpegurl"));
		map.insert(std::make_pair("rdz", "application/vnd.data-vision.rdz"));
		map.insert(std::make_pair("rdf", "application/rdf+xml"));
		map.insert(std::make_pair("jpgm", "video/jpm"));
		map.insert(std::make_pair("dra", "audio/vnd.dra"));
		map.insert(std::make_pair("maker", "application/vnd.framemaker"));
		map.insert(std::make_pair("m3a", "audio/mpeg"));
		map.insert(std::make_pair("fbs", "image/vnd.fastbidsheet"));
		map.insert(std::make_pair("apr", "application/vnd.lotus-approach"));
		map.insert(std::make_pair("txf", "application/vnd.mobius.txf"));
		map.insert(std::make_pair("zir", "application/vnd.zul"));
		map.insert(std::make_pair("txd", "application/vnd.genomatix.tuxedo"));
		map.insert(std::make_pair("f90", "text/x-fortran"));
		map.insert(std::make_pair("apk", "application/vnd.android.package-archive"));
		map.insert(std::make_pair("sgm", "text/sgml"));
		map.insert(std::make_pair("sgl", "application/vnd.stardivision.writer-global"));
		map.insert(std::make_pair("mmd", "application/vnd.chipnuts.karaoke-mmd"));
		map.insert(std::make_pair("txt", "text/plain"));
		map.insert(std::make_pair("wbmp", "image/vnd.wap.wbmp"));
		map.insert(std::make_pair("wsdl", "application/wsdl+xml"));
		map.insert(std::make_pair("tcap", "application/vnd.3gpp2.tcap"));
		map.insert(std::make_pair("c4u", "application/vnd.clonk.c4group"));
		map.insert(std::make_pair("dna", "application/vnd.dna"));
		map.insert(std::make_pair("twd", "application/vnd.simtech-mindmapper"));
		map.insert(std::make_pair("lwp", "application/vnd.lotus-wordpro"));
		map.insert(std::make_pair("chm", "application/vnd.ms-htmlhelp"));
		map.insert(std::make_pair("c4d", "application/vnd.clonk.c4group"));
		map.insert(std::make_pair("xfdf", "application/vnd.adobe.xfdf"));
		map.insert(std::make_pair("c4f", "application/vnd.clonk.c4group"));
		map.insert(std::make_pair("msl", "application/vnd.mobius.msl"));
		map.insert(std::make_pair("rpss", "application/vnd.nokia.radio-presets"));
		map.insert(std::make_pair("msi", "application/x-msdownload"));
		map.insert(std::make_pair("msh", "model/mesh"));
		map.insert(std::make_pair("setreg", "application/set-registration-initiation"));
		map.insert(std::make_pair("msf", "application/vnd.epson.msf"));
		map.insert(std::make_pair("pfa", "application/x-font-type1"));
		map.insert(std::make_pair("x32", "application/x-authorware-bin"));
		map.insert(std::make_pair("wvx", "video/x-ms-wvx"));
		map.insert(std::make_pair("pfb", "application/x-font-type1"));
		map.insert(std::make_pair("wml", "text/vnd.wap.wml"));
		map.insert(std::make_pair("pfm", "application/x-font-type1"));
		map.insert(std::make_pair("fxpl", "application/vnd.adobe.fxp"));
		map.insert(std::make_pair("xvml", "application/xv+xml"));
		map.insert(std::make_pair("lasxml", "application/vnd.las.las+xml"));
		map.insert(std::make_pair("pfr", "application/font-tdpfr"));
		map.insert(std::make_pair("jpg", "image/jpeg"));
		map.insert(std::make_pair("wbxml", "application/vnd.wap.wbxml"));
		map.insert(std::make_pair("pfx", "application/x-pkcs12"));
		map.insert(std::make_pair("fh7", "image/x-freehand"));
		map.insert(std::make_pair("me", "text/troff"));
		map.insert(std::make_pair("flx", "text/vnd.fmi.flexstor"));
		map.insert(std::make_pair("ma", "application/mathematica"));
		map.insert(std::make_pair("mb", "application/mathematica"));
		map.insert(std::make_pair("car", "application/vnd.curl.car"));
		map.insert(std::make_pair("der", "application/x-x509-ca-cert"));
		map.insert(std::make_pair("vcx", "application/vnd.vcx"));
		map.insert(std::make_pair("flw", "application/vnd.kde.kivio"));
		map.insert(std::make_pair("cat", "application/vnd.ms-pki.seccat"));
		map.insert(std::make_pair("fli", "video/x-fli"));
		map.insert(std::make_pair("flo", "application/vnd.micrografx.flo"));
		map.insert(std::make_pair("xht", "application/xhtml+xml"));
		map.insert(std::make_pair("cab", "application/vnd.ms-cab-compressed"));
		map.insert(std::make_pair("deb", "application/x-debian-package"));
		map.insert(std::make_pair("gqs", "application/vnd.grafeq"));
		map.insert(std::make_pair("def", "text/plain"));
		map.insert(std::make_pair("avi", "video/x-msvideo"));
		map.insert(std::make_pair("spx", "audio/ogg"));
		map.insert(std::make_pair("xfdl", "application/vnd.xfdl"));
		map.insert(std::make_pair("wrl", "model/vrml"));
		map.insert(std::make_pair("conf", "text/plain"));
		map.insert(std::make_pair("tar", "application/x-tar"));
		map.insert(std::make_pair("mpg", "video/mpeg"));
		map.insert(std::make_pair("spp", "application/scvp-vp-response"));
		map.insert(std::make_pair("spq", "application/scvp-vp-request"));
		map.insert(std::make_pair("dxr", "application/x-director"));
		map.insert(std::make_pair("dxp", "application/vnd.spotfire.dxp"));
		map.insert(std::make_pair("rif", "application/reginfo+xml"));
		map.insert(std::make_pair("mjp2", "video/mj2"));
		map.insert(std::make_pair("spl", "application/x-futuresplash"));
		map.insert(std::make_pair("dxf", "image/vnd.dxf"));
		map.insert(std::make_pair("tao", "application/vnd.tao.intent-module-archive"));
		map.insert(std::make_pair("spf", "application/vnd.yamaha.smaf-phrase"));
		map.insert(std::make_pair("yang", "application/yang"));
		map.insert(std::make_pair("sis", "application/vnd.symbian.install"));
		map.insert(std::make_pair("afm", "application/x-font-type1"));
		map.insert(std::make_pair("chrt", "application/vnd.kde.kchart"));
		map.insert(std::make_pair("csh", "application/x-csh"));
		map.insert(std::make_pair("sit", "application/x-stuffit"));
		map.insert(std::make_pair("texinfo", "application/x-texinfo"));
		map.insert(std::make_pair("ogv", "video/ogg"));
		map.insert(std::make_pair("wax", "audio/x-ms-wax"));
		map.insert(std::make_pair("sig", "application/pgp-signature"));
		map.insert(std::make_pair("dwg", "image/vnd.dwg"));
		map.insert(std::make_pair("mj2", "video/mj2"));
		map.insert(std::make_pair("cst", "application/x-director"));
		map.insert(std::make_pair("xhtml", "application/xhtml+xml"));
		map.insert(std::make_pair("csv", "text/csv"));
		map.insert(std::make_pair("afp", "application/vnd.ibm.modcap"));
		map.insert(std::make_pair("csp", "application/vnd.commonspace"));
		map.insert(std::make_pair("css", "text/css"));
		map.insert(std::make_pair("wmls", "text/vnd.wap.wmlscript"));
		map.insert(std::make_pair("snf", "application/x-font-snf"));
		map.insert(std::make_pair("pqa", "application/vnd.palm"));
		map.insert(std::make_pair("snd", "audio/basic"));
		map.insert(std::make_pair("spot", "text/vnd.in3d.spot"));
		map.insert(std::make_pair("spc", "application/x-pkcs7-certificates"));
		map.insert(std::make_pair("mesh", "model/mesh"));
		map.insert(std::make_pair("npx", "image/vnd.net-fpx"));
		map.insert(std::make_pair("mag", "application/vnd.ecowin.chart"));
		map.insert(std::make_pair("fh", "image/x-freehand"));
		map.insert(std::make_pair("fm", "application/vnd.framemaker"));
		map.insert(std::make_pair("man", "text/troff"));
		map.insert(std::make_pair("lha", "application/octet-stream"));
		map.insert(std::make_pair("st", "application/vnd.sailingtracker.track"));
		map.insert(std::make_pair("ghf", "application/vnd.groove-help"));
		map.insert(std::make_pair("mgp", "application/vnd.osgeo.mapguide.package"));
		map.insert(std::make_pair("so", "application/octet-stream"));
		map.insert(std::make_pair("sm", "application/vnd.stepmania.stepchart"));
		map.insert(std::make_pair("sc", "application/vnd.ibm.secure-container"));
		map.insert(std::make_pair("uvx", "application/vnd.dece.unspecified"));
		map.insert(std::make_pair("xhvml", "application/xv+xml"));
		map.insert(std::make_pair("mpp", "application/vnd.ms-project"));
		map.insert(std::make_pair("uvp", "video/vnd.dece.pd"));
		map.insert(std::make_pair("uvs", "video/vnd.dece.sd"));
		map.insert(std::make_pair("uvt", "application/vnd.dece.ttml+xml"));
		map.insert(std::make_pair("uvu", "video/vnd.uvvu.mp4"));
		map.insert(std::make_pair("jpeg", "image/jpeg"));
		map.insert(std::make_pair("uvh", "video/vnd.dece.hd"));
		map.insert(std::make_pair("uvi", "image/vnd.dece.graphic"));
		map.insert(std::make_pair("flv", "video/x-flv"));
		map.insert(std::make_pair("list3820", "application/vnd.ibm.modcap"));
		map.insert(std::make_pair("uvm", "video/vnd.dece.mobile"));
		map.insert(std::make_pair("ttl", "text/turtle"));
		map.insert(std::make_pair("ttc", "application/x-font-ttf"));
		map.insert(std::make_pair("uva", "audio/vnd.dece.audio"));
		map.insert(std::make_pair("uvd", "application/vnd.dece.data"));
		map.insert(std::make_pair("ttf", "application/x-font-ttf"));
		map.insert(std::make_pair("uvf", "application/vnd.dece.data"));
		map.insert(std::make_pair("xo", "application/vnd.olpc-sugar"));
		map.insert(std::make_pair("joda", "application/vnd.joost.joda-archive"));
		map.insert(std::make_pair("pcx", "image/x-pcx"));
		map.insert(std::make_pair("h263", "video/h263"));
		map.insert(std::make_pair("djv", "image/vnd.djvu"));
		map.insert(std::make_pair("tsd", "application/timestamped-data"));
		map.insert(std::make_pair("mbk", "application/vnd.mobius.mbk"));
		map.insert(std::make_pair("cla", "application/vnd.claymore"));
		map.insert(std::make_pair("pct", "image/x-pict"));
		map.insert(std::make_pair("wri", "application/x-mswrite"));
		map.insert(std::make_pair("imp", "application/vnd.accpac.simply.imp"));
		map.insert(std::make_pair("ims", "application/vnd.ms-ims"));
		map.insert(std::make_pair("vtu", "model/vnd.vtu"));
		map.insert(std::make_pair("fig", "application/x-xfig"));
		map.insert(std::make_pair("pcl", "application/vnd.hp-pcl"));
		map.insert(std::make_pair("gtw", "model/vnd.gtw"));
		map.insert(std::make_pair("tsv", "text/tab-separated-values"));
		map.insert(std::make_pair("dwf", "model/vnd.dwf"));
		map.insert(std::make_pair("silo", "model/mesh"));
		map.insert(std::make_pair("pcf", "application/x-font-pcf"));
		map.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
		map.insert(std::make_pair("mp2a", "audio/mpeg"));
		map.insert(std::make_pair("daf", "application/vnd.mobius.daf"));
		map.insert(std::make_pair("cxx", "text/x-c"));
		map.insert(std::make_pair("mp4a", "audio/mp4"));
		map.insert(std::make_pair("mseed", "application/vnd.fdsn.mseed"));
		map.insert(std::make_pair("mxs", "application/vnd.triscape.mxs"));
		map.insert(std::make_pair("p10", "application/pkcs10"));
		map.insert(std::make_pair("lzh", "application/octet-stream"));
		map.insert(std::make_pair("mxl", "application/vnd.recordare.musicxml"));
		map.insert(std::make_pair("cer", "application/pkix-cert"));
		map.insert(std::make_pair("docm", "application/vnd.ms-word.document.macroenabled.12"));
		map.insert(std::make_pair("mxf", "application/mxf"));
		map.insert(std::make_pair("qt", "video/quicktime"));
		map.insert(std::make_pair("hqx", "application/mac-binhex40"));
		map.insert(std::make_pair("pps", "application/vnd.ms-powerpoint"));
		map.insert(std::make_pair("xlm", "application/vnd.ms-excel"));
		map.insert(std::make_pair("mods", "application/mods+xml"));
		map.insert(std::make_pair("kpt", "application/vnd.kde.kpresenter"));
		map.insert(std::make_pair("f4v", "video/x-f4v"));
		map.insert(std::make_pair("xlc", "application/vnd.ms-excel"));
		map.insert(std::make_pair("xla", "application/vnd.ms-excel"));
		map.insert(std::make_pair("fpx", "image/vnd.fpx"));
		map.insert(std::make_pair("ifb", "text/calendar"));
		map.insert(std::make_pair("h", "text/x-c"));
		map.insert(std::make_pair("xlw", "application/vnd.ms-excel"));
		map.insert(std::make_pair("ifm", "application/vnd.shana.informed.formdata"));
		map.insert(std::make_pair("xlt", "application/vnd.ms-excel"));
		map.insert(std::make_pair("xls", "application/vnd.ms-excel"));
		map.insert(std::make_pair("atomcat", "application/atomcat+xml"));
		map.insert(std::make_pair("mets", "application/mets+xml"));
		map.insert(std::make_pair("mdi", "image/vnd.ms-modi"));
		map.insert(std::make_pair("uvvu", "video/vnd.uvvu.mp4"));
		map.insert(std::make_pair("p", "text/x-pascal"));
		map.insert(std::make_pair("mrcx", "application/marcxml+xml"));
		map.insert(std::make_pair("list", "text/plain"));
		map.insert(std::make_pair("hlp", "application/winhlp"));
		map.insert(std::make_pair("gtar", "application/x-gtar"));
		map.insert(std::make_pair("ez", "application/andrew-inset"));
		map.insert(std::make_pair("mxml", "application/xv+xml"));
		map.insert(std::make_pair("dic", "text/x-c"));
		map.insert(std::make_pair("wqd", "application/vnd.wqd"));
		map.insert(std::make_pair("vcd", "application/x-cdlink"));
		map.insert(std::make_pair("rq", "application/sparql-query"));
		map.insert(std::make_pair("vcf", "text/x-vcard"));
		map.insert(std::make_pair("rs", "application/rls-services+xml"));
		map.insert(std::make_pair("res", "application/x-dtbresource+xml"));
		map.insert(std::make_pair("rep", "application/vnd.businessobjects"));
		map.insert(std::make_pair("cww", "application/prs.cww"));
		map.insert(std::make_pair("wgt", "application/widget"));
		map.insert(std::make_pair("ftc", "application/vnd.fluxtime.clip"));
		map.insert(std::make_pair("vcs", "text/x-vcalendar"));
		map.insert(std::make_pair("ra", "audio/x-pn-realaudio"));
		map.insert(std::make_pair("smi", "application/smil+xml"));
		map.insert(std::make_pair("3gp", "video/3gpp"));
		map.insert(std::make_pair("fti", "application/vnd.anser-web-funds-transfer-initiation"));
		map.insert(std::make_pair("rm", "application/vnd.rn-realmedia"));
		map.insert(std::make_pair("roff", "text/troff"));
		map.insert(std::make_pair("m2a", "audio/mpeg"));
		map.insert(std::make_pair("dataless", "application/vnd.fdsn.seed"));
		map.insert(std::make_pair("crd", "application/x-mscardfile"));
		map.insert(std::make_pair("rld", "application/resource-lists-diff+xml"));
		map.insert(std::make_pair("cmx", "image/x-cmx"));
		map.insert(std::make_pair("p7r", "application/x-pkcs7-certreqresp"));
		map.insert(std::make_pair("pub", "application/x-mspublisher"));
		map.insert(std::make_pair("sh", "application/x-sh"));
		map.insert(std::make_pair("asc", "application/pgp-signature"));
		map.insert(std::make_pair("mime", "message/rfc822"));
		map.insert(std::make_pair("asf", "video/x-ms-asf"));
		map.insert(std::make_pair("org", "application/vnd.lotus-organizer"));
		map.insert(std::make_pair("mmf", "application/vnd.smaf"));
		map.insert(std::make_pair("aso", "application/vnd.accpac.simply.aso"));
		map.insert(std::make_pair("asm", "text/x-asm"));
		map.insert(std::make_pair("log", "text/plain"));
		map.insert(std::make_pair("java", "text/x-java-source"));
		map.insert(std::make_pair("pkipath", "application/pkix-pkipath"));
		map.insert(std::make_pair("atc", "application/vnd.acucorp"));
		map.insert(std::make_pair("midi", "audio/midi"));
		map.insert(std::make_pair("potx", "application/vnd.openxmlformats-officedocument.presentationml.template"));
		map.insert(std::make_pair("m21", "application/mp21"));
		map.insert(std::make_pair("mvb", "application/x-msmediaview"));
		map.insert(std::make_pair("iso", "application/octet-stream"));
		map.insert(std::make_pair("ltf", "application/vnd.frogans.ltf"));
		map.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
		map.insert(std::make_pair("cdkey", "application/vnd.mediastation.cdkey"));
		map.insert(std::make_pair("gif", "image/gif"));
		map.insert(std::make_pair("atx", "application/vnd.antix.game-component"));
		map.insert(std::make_pair("uvva", "audio/vnd.dece.audio"));
		map.insert(std::make_pair("uvvg", "image/vnd.dece.graphic"));
		map.insert(std::make_pair("uvvf", "application/vnd.dece.data"));
		map.insert(std::make_pair("uvvd", "application/vnd.dece.data"));
		map.insert(std::make_pair("pgn", "application/x-chess-pgn"));
		map.insert(std::make_pair("uvvi", "image/vnd.dece.graphic"));
		map.insert(std::make_pair("pgm", "image/x-portable-graymap"));
		map.insert(std::make_pair("htke", "application/vnd.kenameaapp"));
		map.insert(std::make_pair("uvvm", "video/vnd.dece.mobile"));
		map.insert(std::make_pair("uvvs", "video/vnd.dece.sd"));
		map.insert(std::make_pair("uvvp", "video/vnd.dece.pd"));
		map.insert(std::make_pair("zaz", "application/vnd.zzazz.deck+xml"));
		map.insert(std::make_pair("uvvv", "video/vnd.dece.video"));
		map.insert(std::make_pair("pgp", "application/pgp-encrypted"));
		map.insert(std::make_pair("uvvt", "application/vnd.dece.ttml+xml"));
		map.insert(std::make_pair("rcprofile", "application/vnd.ipunplugged.rcprofile"));
		map.insert(std::make_pair("uvvx", "application/vnd.dece.unspecified"));
		map.insert(std::make_pair("gph", "application/vnd.flographit"));
		map.insert(std::make_pair("xif", "image/vnd.xiff"));
		map.insert(std::make_pair("kon", "application/vnd.kde.kontour"));
		map.insert(std::make_pair("yin", "application/yin+xml"));
		map.insert(std::make_pair("ptid", "application/vnd.pvi.ptid1"));
		map.insert(std::make_pair("potm", "application/vnd.ms-powerpoint.template.macroenabled.12"));
		map.insert(std::make_pair("svgz", "image/svg+xml"));
		map.insert(std::make_pair("cdxml", "application/vnd.chemdraw+xml"));
		map.insert(std::make_pair("uris", "text/uri-list"));
		map.insert(std::make_pair("ncx", "application/x-dtbncx+xml"));
		map.insert(std::make_pair("nbp", "application/vnd.wolfram.player"));
		map.insert(std::make_pair("tfi", "application/thraud+xml"));
		map.insert(std::make_pair("ogx", "application/ogg"));
		map.insert(std::make_pair("rsd", "application/rsd+xml"));
		map.insert(std::make_pair("bcpio", "application/x-bcpio"));
		map.insert(std::make_pair("zirz", "application/vnd.zul"));
		map.insert(std::make_pair("onetoc2", "application/onenote"));
		map.insert(std::make_pair("p12", "application/x-pkcs12"));
		map.insert(std::make_pair("pnm", "image/x-portable-anymap"));
		map.insert(std::make_pair("wav", "audio/x-wav"));
		map.insert(std::make_pair("ogg", "audio/ogg"));
		map.insert(std::make_pair("oga", "audio/ogg"));
		map.insert(std::make_pair("png", "image/png"));
		map.insert(std::make_pair("rss", "application/rss+xml"));
		map.insert(std::make_pair("std", "application/vnd.sun.xml.draw.template"));
		map.insert(std::make_pair("onetmp", "application/onenote"));
		map.insert(std::make_pair("stf", "application/vnd.wt.stf"));
		map.insert(std::make_pair("ktx", "image/ktx"));
		map.insert(std::make_pair("ktz", "application/vnd.kahootz"));
		map.insert(std::make_pair("odft", "application/vnd.oasis.opendocument.formula-template"));
		map.insert(std::make_pair("stl", "application/vnd.ms-pki.stl"));
		map.insert(std::make_pair("dtb", "application/x-dtbook+xml"));
		map.insert(std::make_pair("sti", "application/vnd.sun.xml.impress.template"));
		map.insert(std::make_pair("ktr", "application/vnd.kahootz"));
		map.insert(std::make_pair("stk", "application/hyperstudio"));
		map.insert(std::make_pair("stw", "application/vnd.sun.xml.writer.template"));
		map.insert(std::make_pair("str", "application/vnd.pg.format"));
		map.insert(std::make_pair("dts", "audio/vnd.dts"));
		map.insert(std::make_pair("fe_launch", "application/vnd.denovo.fcselayout-link"));
		map.insert(std::make_pair("torrent", "application/x-bittorrent"));
		map.insert(std::make_pair("mov", "video/quicktime"));
		map.insert(std::make_pair("ecelp9600", "audio/vnd.nuera.ecelp9600"));
		map.insert(std::make_pair("pre", "application/vnd.lotus-freelance"));
		map.insert(std::make_pair("tfm", "application/x-tex-tfm"));
		map.insert(std::make_pair("prf", "application/pics-rules"));
		map.insert(std::make_pair("n-gage", "application/vnd.nokia.n-gage.symbian.install"));
		map.insert(std::make_pair("prc", "application/x-mobipocket-ebook"));
		map.insert(std::make_pair("ram", "audio/x-pn-realaudio"));
		map.insert(std::make_pair("saf", "application/vnd.yamaha.smaf-audio"));
		map.insert(std::make_pair("seed", "application/vnd.fdsn.seed"));
		map.insert(std::make_pair("lbe", "application/vnd.llamagraphics.life-balance.exchange+xml"));
		map.insert(std::make_pair("lbd", "application/vnd.llamagraphics.life-balance.desktop"));
		map.insert(std::make_pair("qps", "application/vnd.publishare-delta-tree"));
		map.insert(std::make_pair("atomsvc", "application/atomsvc+xml"));
		map.insert(std::make_pair("7z", "application/x-7z-compressed"));
		map.insert(std::make_pair("mseq", "application/vnd.mseq"));
		map.insert(std::make_pair("rmi", "audio/midi"));
		map.insert(std::make_pair("bz2", "application/x-bzip2"));
		map.insert(std::make_pair("3dml", "text/vnd.in3d.3dml"));
		map.insert(std::make_pair("ngdat", "application/vnd.nokia.n-gage.data"));
		map.insert(std::make_pair("fxp", "application/vnd.adobe.fxp"));
		map.insert(std::make_pair("teicorpus", "application/tei+xml"));
		map.insert(std::make_pair("listafp", "application/vnd.ibm.modcap"));
		map.insert(std::make_pair("stc", "application/vnd.sun.xml.calc.template"));
		map.insert(std::make_pair("meta4", "application/metalink4+xml"));
		map.insert(std::make_pair("aifc", "audio/x-aiff"));
		map.insert(std::make_pair("wcm", "application/vnd.ms-works"));
		map.insert(std::make_pair("csml", "chemical/x-csml"));
		map.insert(std::make_pair("fcs", "application/vnd.isac.fcs"));
		map.insert(std::make_pair("xdssc", "application/dssc+xml"));
		map.insert(std::make_pair("deploy", "application/octet-stream"));
		map.insert(std::make_pair("rpst", "application/vnd.nokia.radio-preset"));
		map.insert(std::make_pair("sfs", "application/vnd.spotfire.sfs"));
		map.insert(std::make_pair("pyv", "video/vnd.ms-playready.media.pyv"));
		map.insert(std::make_pair("pptm", "application/vnd.ms-powerpoint.presentation.macroenabled.12"));
		map.insert(std::make_pair("kar", "audio/midi"));
		map.insert(std::make_pair("mpeg", "video/mpeg"));
		map.insert(std::make_pair("sisx", "application/vnd.symbian.install"));
		map.insert(std::make_pair("pya", "audio/vnd.ms-playready.media.pya"));
		map.insert(std::make_pair("bh2", "application/vnd.fujitsu.oasysprs"));
		map.insert(std::make_pair("class", "application/java-vm"));
		map.insert(std::make_pair("epub", "application/epub+zip"));
		map.insert(std::make_pair("latex", "application/x-latex"));
		map.insert(std::make_pair("exe", "application/x-msdownload"));
		map.insert(std::make_pair("doc", "application/msword"));
		map.insert(std::make_pair("tpl", "application/vnd.groove-tool-template"));
		map.insert(std::make_pair("uri", "text/uri-list"));
		map.insert(std::make_pair("exi", "application/exi"));
		map.insert(std::make_pair("ext", "application/vnd.novadigm.ext"));
		map.insert(std::make_pair("tpt", "application/vnd.trid.tpt"));
		map.insert(std::make_pair("efif", "application/vnd.picsel"));
		map.insert(std::make_pair("dot", "application/msword"));
		map.insert(std::make_pair("qbo", "application/vnd.intu.qbo"));
		map.insert(std::make_pair("cdf", "application/x-netcdf"));
		map.insert(std::make_pair("rtf", "application/rtf"));
		map.insert(std::make_pair("nlu", "application/vnd.neurolanguage.nlu"));
		map.insert(std::make_pair("text", "text/plain"));
		map.insert(std::make_pair("sdc", "application/vnd.stardivision.calc"));
		map.insert(std::make_pair("pskcxml", "application/pskc+xml"));
		map.insert(std::make_pair("texi", "application/x-texinfo"));
		map.insert(std::make_pair("pkg", "application/octet-stream"));
		map.insert(std::make_pair("itp", "application/vnd.shana.informed.formtemplate"));
		map.insert(std::make_pair("pki", "application/pkixcmp"));
		map.insert(std::make_pair("cdx", "chemical/x-cdx"));
		map.insert(std::make_pair("cdy", "application/vnd.cinderella"));
		map.insert(std::make_pair("xml", "application/xml"));
		map.insert(std::make_pair("ksp", "application/vnd.kde.kspread"));
		map.insert(std::make_pair("jar", "application/java-archive"));
		map.insert(std::make_pair("mp21", "application/mp21"));
		map.insert(std::make_pair("jam", "application/vnd.jam"));
		map.insert(std::make_pair("jad", "text/vnd.sun.j2me.app-descriptor"));
		map.insert(std::make_pair("ief", "image/ief"));
		map.insert(std::make_pair("xdf", "application/xcap-diff+xml"));
		map.insert(std::make_pair("xdm", "application/vnd.syncml.dm+xml"));
		map.insert(std::make_pair("gex", "application/vnd.geometry-explorer"));
		map.insert(std::make_pair("cpio", "application/x-cpio"));
		map.insert(std::make_pair("xdw", "application/vnd.fujixerox.docuworks"));
		map.insert(std::make_pair("123", "application/vnd.lotus-1-2-3"));
		map.insert(std::make_pair("tbm", "text/plain"));
		map.insert(std::make_pair("tbl", "text/plain"));
		map.insert(std::make_pair("xdp", "application/vnd.adobe.xdp+xml"));
		map.insert(std::make_pair("sse", "application/vnd.kodak-descriptor"));
		map.insert(std::make_pair("mgz", "application/vnd.proteus.magazine"));
		map.insert(std::make_pair("ssf", "application/vnd.epson.ssf"));
		map.insert(std::make_pair("geo", "application/vnd.dynageo"));
		map.insert(std::make_pair("dp", "application/vnd.osgi.dp"));
		map.insert(std::make_pair("cif", "chemical/x-cif"));
		map.insert(std::make_pair("musicxml", "application/vnd.recordare.musicxml+xml"));
		map.insert(std::make_pair("aep", "application/vnd.audiograph"));
		map.insert(std::make_pair("bat", "application/x-msdownload"));
		map.insert(std::make_pair("aac", "audio/x-aac"));
		map.insert(std::make_pair("crt", "application/x-x509-ca-cert"));
		map.insert(std::make_pair("oxt", "application/vnd.openofficeorg.extension"));
		map.insert(std::make_pair("eml", "message/rfc822"));
		map.insert(std::make_pair("fdf", "application/vnd.fdf"));
		map.insert(std::make_pair("uvv", "video/vnd.dece.video"));
		map.insert(std::make_pair("dpg", "application/vnd.dpgraph"));
		map.insert(std::make_pair("aam", "application/x-authorware-map"));
		map.insert(std::make_pair("gram", "application/srgs"));
		map.insert(std::make_pair("shf", "application/shf+xml"));
		map.insert(std::make_pair("crl", "application/pkix-crl"));
		map.insert(std::make_pair("rms", "application/vnd.jcp.javame.midlet-rms"));
		map.insert(std::make_pair("rmp", "audio/x-pn-realaudio-plugin"));
		map.insert(std::make_pair("ser", "application/java-serialized-object"));
		map.insert(std::make_pair("dump", "application/octet-stream"));
		map.insert(std::make_pair("fg5", "application/vnd.fujitsu.oasysgp"));
		map.insert(std::make_pair("frame", "application/vnd.framemaker"));
		map.insert(std::make_pair("vis", "application/vnd.visionary"));
		map.insert(std::make_pair("see", "application/vnd.seemail"));
		map.insert(std::make_pair("pvb", "application/vnd.3gpp.pic-bw-var"));
		map.insert(std::make_pair("hpgl", "application/vnd.hp-hpgl"));
		map.insert(std::make_pair("cryptonote", "application/vnd.rig.cryptonote"));
		map.insert(std::make_pair("dtd", "application/xml-dtd"));
		map.insert(std::make_pair("susp", "application/vnd.sus-calendar"));
		map.insert(std::make_pair("cmp", "application/vnd.yellowriver-custom-menu"));
		map.insert(std::make_pair("et3", "application/vnd.eszigno3+xml"));
		map.insert(std::make_pair("mus", "application/vnd.musician"));
		map.insert(std::make_pair("irp", "application/vnd.irepository.package+xml"));
		map.insert(std::make_pair("movie", "video/x-sgi-movie"));
		map.insert(std::make_pair("osfpvg", "application/vnd.yamaha.openscoreformat.osfpvg+xml"));
		map.insert(std::make_pair("xvm", "application/xv+xml"));
		map.insert(std::make_pair("irm", "application/vnd.ibm.rights-management"));
		map.insert(std::make_pair("otc", "application/vnd.oasis.opendocument.chart-template"));
		map.insert(std::make_pair("uvg", "image/vnd.dece.graphic"));
		map.insert(std::make_pair("wg", "application/vnd.pmi.widget"));
		map.insert(std::make_pair("cdmia", "application/cdmi-capability"));
		map.insert(std::make_pair("cdmic", "application/cdmi-container"));
		map.insert(std::make_pair("cdmid", "application/cdmi-domain"));
		map.insert(std::make_pair("bed", "application/vnd.realvnc.bed"));
		map.insert(std::make_pair("xslt", "application/xslt+xml"));
		map.insert(std::make_pair("h264", "video/h264"));
		map.insert(std::make_pair("boz", "application/x-bzip2"));
		map.insert(std::make_pair("wm", "video/x-ms-wm"));
		map.insert(std::make_pair("f77", "text/x-fortran"));
		map.insert(std::make_pair("cdmio", "application/cdmi-object"));
		map.insert(std::make_pair("cdmiq", "application/cdmi-queue"));
		map.insert(std::make_pair("wtb", "application/vnd.webturbo"));
		map.insert(std::make_pair("fgd", "application/x-director"));
		map.insert(std::make_pair("js", "application/javascript"));
		map.insert(std::make_pair("c", "text/x-c"));
		map.insert(std::make_pair("qfx", "application/vnd.intu.qfx"));
		map.insert(std::make_pair("adp", "audio/adpcm"));
		map.insert(std::make_pair("etx", "text/x-setext"));
		map.insert(std::make_pair("xlam", "application/vnd.ms-excel.addin.macroenabled.12"));
		map.insert(std::make_pair("s", "text/x-asm"));
		map.insert(std::make_pair("fnc", "application/vnd.frogans.fnc"));
		map.insert(std::make_pair("cod", "application/vnd.rim.cod"));
		map.insert(std::make_pair("urls", "text/uri-list"));
		map.insert(std::make_pair("pdf", "application/pdf"));
		map.insert(std::make_pair("com", "application/x-msdownload"));
		map.insert(std::make_pair("pdb", "application/vnd.palm"));
		map.insert(std::make_pair("ecma", "application/ecmascript"));
		map.insert(std::make_pair("xltm", "application/vnd.ms-excel.template.macroenabled.12"));
		map.insert(std::make_pair("cxt", "application/x-director"));
		map.insert(std::make_pair("kwd", "application/vnd.kde.kword"));
		map.insert(std::make_pair("cc", "text/x-c"));
		map.insert(std::make_pair("pot", "application/vnd.ms-powerpoint"));
		map.insert(std::make_pair("kfo", "application/vnd.kde.kformula"));
		map.insert(std::make_pair("hvd", "application/vnd.yamaha.hv-dic"));
		map.insert(std::make_pair("gtm", "application/vnd.groove-tool-message"));
		map.insert(std::make_pair("gxt", "application/vnd.geonext"));
		map.insert(std::make_pair("m13", "application/x-msmediaview"));
		map.insert(std::make_pair("xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template"));
		map.insert(std::make_pair("kwt", "application/vnd.kde.kword"));
		map.insert(std::make_pair("hvp", "application/vnd.yamaha.hv-voice"));
		map.insert(std::make_pair("hvs", "application/vnd.yamaha.hv-script"));
		map.insert(std::make_pair("cu", "application/cu-seeme"));
		map.insert(std::make_pair("ppsx", "application/vnd.openxmlformats-officedocument.presentationml.slideshow"));
		map.insert(std::make_pair("swi", "application/vnd.aristanetworks.swi"));
		map.insert(std::make_pair("ps", "application/postscript"));
		map.insert(std::make_pair("sv4crc", "application/x-sv4crc"));
		map.insert(std::make_pair("dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template"));
		map.insert(std::make_pair("ei6", "application/vnd.pg.osasli"));
		map.insert(std::make_pair("swa", "application/x-director"));
		map.insert(std::make_pair("xar", "application/vnd.xara"));
		map.insert(std::make_pair("dis", "application/vnd.mobius.dis"));
		map.insert(std::make_pair("swf", "application/x-shockwave-flash"));
		map.insert(std::make_pair("json", "application/json"));
		map.insert(std::make_pair("hpid", "application/vnd.hp-hpid"));
		map.insert(std::make_pair("oth", "application/vnd.oasis.opendocument.text-web"));
		map.insert(std::make_pair("oti", "application/vnd.oasis.opendocument.image-template"));
		map.insert(std::make_pair("nnd", "application/vnd.noblenet-directory"));
		map.insert(std::make_pair("rp9", "application/vnd.cloanto.rp9"));
		map.insert(std::make_pair("c4g", "application/vnd.clonk.c4group"));
		map.insert(std::make_pair("mcd", "application/vnd.mcd"));
		map.insert(std::make_pair("sldx", "application/vnd.openxmlformats-officedocument.presentationml.slide"));
		map.insert(std::make_pair("otf", "application/x-font-otf"));
		map.insert(std::make_pair("otg", "application/vnd.oasis.opendocument.graphics-template"));
		map.insert(std::make_pair("bmp", "image/bmp"));
		map.insert(std::make_pair("xlsm", "application/vnd.ms-excel.sheet.macroenabled.12"));
		map.insert(std::make_pair("m1v", "video/mpeg"));
		map.insert(std::make_pair("sldm", "application/vnd.ms-powerpoint.slide.macroenabled.12"));
		map.insert(std::make_pair("xlsb", "application/vnd.ms-excel.sheet.binary.macroenabled.12"));
		map.insert(std::make_pair("gac", "application/vnd.groove-account"));
		map.insert(std::make_pair("dotm", "application/vnd.ms-word.template.macroenabled.12"));
		map.insert(std::make_pair("slt", "application/vnd.epson.salt"));
		map.insert(std::make_pair("ait", "application/vnd.dvb.ait"));
		map.insert(std::make_pair("p8", "application/pkcs8"));
		map.insert(std::make_pair("air", "application/vnd.adobe.air-application-installer-package+zip"));
		map.insert(std::make_pair("hdf", "application/x-hdf"));
		map.insert(std::make_pair("mlp", "application/vnd.dolby.mlp"));
		map.insert(std::make_pair("sdkd", "application/vnd.solent.sdkm+xml"));
		map.insert(std::make_pair("mpg4", "video/mp4"));
		map.insert(std::make_pair("rar", "application/x-rar-compressed"));
		map.insert(std::make_pair("aif", "audio/x-aiff"));
		map.insert(std::make_pair("mbox", "application/mbox"));
		map.insert(std::make_pair("sdkm", "application/vnd.solent.sdkm+xml"));
		map.insert(std::make_pair("ras", "image/x-cmu-raster"));
		map.insert(std::make_pair("ami", "application/vnd.amiga.ami"));
		map.insert(std::make_pair("jpm", "video/jpm"));
		map.insert(std::make_pair("chat", "application/x-chat"));
		map.insert(std::make_pair("dae", "model/vnd.collada+xml"));
		map.insert(std::make_pair("3g2", "video/3gpp2"));
		map.insert(std::make_pair("jpe", "image/jpeg"));
		map.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
		map.insert(std::make_pair("rip", "audio/vnd.rip"));
		map.insert(std::make_pair("sitx", "application/x-stuffitx"));
		map.insert(std::make_pair("ivu", "application/vnd.immervision-ivu"));
		map.insert(std::make_pair("ivp", "application/vnd.immervision-ivp"));
		map.insert(std::make_pair("jpgv", "video/jpeg"));
		map.insert(std::make_pair("vor", "application/vnd.stardivision.writer"));
		map.insert(std::make_pair("in", "text/plain"));
		map.insert(std::make_pair("mqy", "application/vnd.mobius.mqy"));
		map.insert(std::make_pair("wks", "application/vnd.ms-works"));
		map.insert(std::make_pair("vox", "application/x-authorware-bin"));
		map.insert(std::make_pair("shar", "application/x-shar"));
		map.insert(std::make_pair("wmx", "video/x-ms-wmx"));
		map.insert(std::make_pair("pas", "text/x-pascal"));
		map.insert(std::make_pair("paw", "application/vnd.pawaafile"));
		map.insert(std::make_pair("psb", "application/vnd.3gpp.pic-bw-small"));
		map.insert(std::make_pair("html", "text/html"));
		map.insert(std::make_pair("wspolicy", "application/wspolicy+xml"));
		map.insert(std::make_pair("qam", "application/vnd.epson.quickanime"));
		map.insert(std::make_pair("gnumeric", "application/x-gnumeric"));
		map.insert(std::make_pair("mxu", "video/vnd.mpegurl"));
		map.insert(std::make_pair("xbap", "application/x-ms-xbap"));
		map.insert(std::make_pair("wps", "application/vnd.ms-works"));
		map.insert(std::make_pair("wpl", "application/vnd.ms-wpl"));
		map.insert(std::make_pair("unityweb", "application/vnd.unity"));
		map.insert(std::make_pair("ecelp7470", "audio/vnd.nuera.ecelp7470"));
		map.insert(std::make_pair("wpd", "application/vnd.wordperfect"));
		map.insert(std::make_pair("kia", "application/vnd.kidspiration"));
		map.insert(std::make_pair("mobi", "application/x-mobipocket-ebook"));
		map.insert(std::make_pair("karbon", "application/vnd.kde.karbon"));
		map.insert(std::make_pair("cct", "application/x-director"));
		map.insert(std::make_pair("kne", "application/vnd.kinar"));
		map.insert(std::make_pair("eps", "application/postscript"));
		map.insert(std::make_pair("gsf", "application/x-font-ghostscript"));
		map.insert(std::make_pair("cdbcmsg", "application/vnd.contact.cmsg"));
		map.insert(std::make_pair("wmlc", "application/vnd.wap.wmlc"));
		map.insert(std::make_pair("knp", "application/vnd.kinar"));
		map.insert(std::make_pair("i2g", "application/vnd.intergeo"));
		map.insert(std::make_pair("mp4v", "video/mp4"));
		map.insert(std::make_pair("mp4s", "application/mp4"));
		map.insert(std::make_pair("xspf", "application/xspf+xml"));
		map.insert(std::make_pair("aiff", "audio/x-aiff"));
		map.insert(std::make_pair("davmount", "application/davmount+xml"));
		map.insert(std::make_pair("mfm", "application/vnd.mfmp"));
		map.insert(std::make_pair("mid", "audio/midi"));
		map.insert(std::make_pair("ecelp4800", "audio/vnd.nuera.ecelp4800"));
		map.insert(std::make_pair("tcl", "application/x-tcl"));
		map.insert(std::make_pair("xer", "application/patch-ops-error+xml"));
		map.insert(std::make_pair("mif", "application/vnd.mif"));
		map.insert(std::make_pair("gdl", "model/vnd.gdl"));
		map.insert(std::make_pair("msty", "application/vnd.muvee.style"));
		map.insert(std::make_pair("skt", "application/vnd.koan"));
		map.insert(std::make_pair("hal", "application/vnd.hal+xml"));
		map.insert(std::make_pair("skp", "application/vnd.koan"));
		map.insert(std::make_pair("g3w", "application/vnd.geospace"));
		map.insert(std::make_pair("elc", "application/octet-stream"));
		map.insert(std::make_pair("dtshd", "audio/vnd.dts.hd"));
		map.insert(std::make_pair("m4v", "video/x-m4v"));
		map.insert(std::make_pair("m4u", "video/vnd.mpegurl"));
		map.insert(std::make_pair("skd", "application/vnd.koan"));
		map.insert(std::make_pair("rgb", "image/x-rgb"));
		map.insert(std::make_pair("gmx", "application/vnd.gmx"));
		map.insert(std::make_pair("ipk", "application/vnd.shana.informed.package"));
		map.insert(std::make_pair("skm", "application/vnd.koan"));
		map.insert(std::make_pair("ccxml", "application/ccxml+xml"));
		map.insert(std::make_pair("fvt", "video/vnd.fvt"));
		map.insert(std::make_pair("xenc", "application/xenc+xml"));
		map.insert(std::make_pair("bin", "application/octet-stream"));
		map.insert(std::make_pair("dsc", "text/prs.lines.tag"));
		map.insert(std::make_pair("ufdl", "application/vnd.ufdl"));
		map.insert(std::make_pair("rnc", "application/relax-ng-compact-syntax"));
		map.insert(std::make_pair("qwd", "application/vnd.quark.quarkxpress"));
		map.insert(std::make_pair("psf", "application/x-font-linux-psf"));
		map.insert(std::make_pair("kpr", "application/vnd.kde.kpresenter"));
		map.insert(std::make_pair("lostxml", "application/lost+xml"));
		map.insert(std::make_pair("opf", "application/oebps-package+xml"));
		map.insert(std::make_pair("ustar", "application/x-ustar"));
		map.insert(std::make_pair("qwt", "application/vnd.quark.quarkxpress"));
		map.insert(std::make_pair("tif", "image/tiff"));
		map.insert(std::make_pair("c11amz", "application/vnd.cluetrust.cartomobile-config-pkg"));
		map.insert(std::make_pair("qxb", "application/vnd.quark.quarkxpress"));
		map.insert(std::make_pair("emma", "application/emma+xml"));
		map.insert(std::make_pair("wbs", "application/vnd.criticaltools.wbs+xml"));
		map.insert(std::make_pair("qxd", "application/vnd.quark.quarkxpress"));
		map.insert(std::make_pair("acutc", "application/vnd.acucorp"));
		map.insert(std::make_pair("oprc", "application/vnd.palm"));
		map.insert(std::make_pair("semf", "application/vnd.semf"));
		map.insert(std::make_pair("bpk", "application/octet-stream"));
		map.insert(std::make_pair("semd", "application/vnd.semd"));
		map.insert(std::make_pair("qxl", "application/vnd.quark.quarkxpress"));
		map.insert(std::make_pair("t", "text/troff"));
		map.insert(std::make_pair("qxt", "application/vnd.quark.quarkxpress"));
		map.insert(std::make_pair("viv", "video/vnd.vivo"));
		map.insert(std::make_pair("c11amc", "application/vnd.cluetrust.cartomobile-config"));
		map.insert(std::make_pair("xsl", "application/xml"));
		map.insert(std::make_pair("xsm", "application/vnd.syncml+xml"));
		map.insert(std::make_pair("scurl", "text/vnd.curl.scurl"));
		map.insert(std::make_pair("jlt", "application/vnd.hp-jlyt"));
		map.insert(std::make_pair("vss", "application/vnd.visio"));
		map.insert(std::make_pair("vst", "application/vnd.visio"));
		map.insert(std::make_pair("vsw", "application/vnd.visio"));
		map.insert(std::make_pair("webm", "video/webm"));
		map.insert(std::make_pair("webp", "image/webp"));
		map.insert(std::make_pair("sxg", "application/vnd.sun.xml.writer.global"));
		map.insert(std::make_pair("sema", "application/vnd.sema"));
		map.insert(std::make_pair("vsf", "application/vnd.vsf"));
		map.insert(std::make_pair("udeb", "application/x-debian-package"));
		map.insert(std::make_pair("psd", "image/vnd.adobe.photoshop"));
		map.insert(std::make_pair("umj", "application/vnd.umajin"));
		map.insert(std::make_pair("ico", "image/x-icon"));
		map.insert(std::make_pair("icm", "application/vnd.iccprofile"));
		map.insert(std::make_pair("icc", "application/vnd.iccprofile"));
		map.insert(std::make_pair("kml", "application/vnd.google-earth.kml+xml"));
		map.insert(std::make_pair("ice", "x-conference/x-cooltalk"));
		map.insert(std::make_pair("n3", "text/n3"));
		map.insert(std::make_pair("esf", "application/vnd.epson.esf"));
		map.insert(std::make_pair("cgm", "image/cgm"));
		map.insert(std::make_pair("aab", "application/x-authorware-bin"));
		map.insert(std::make_pair("mc1", "application/vnd.medcalcdata"));
		map.insert(std::make_pair("x3d", "application/vnd.hzn-3d-crossword"));
		map.insert(std::make_pair("ics", "text/calendar"));
		map.insert(std::make_pair("kmz", "application/vnd.google-earth.kmz"));
		map.insert(std::make_pair("ddd", "application/vnd.fujixerox.ddd"));
		map.insert(std::make_pair("jnlp", "application/x-java-jnlp-file"));
		map.insert(std::make_pair("pls", "application/pls+xml"));
		map.insert(std::make_pair("m3u8", "application/vnd.apple.mpegurl"));
		map.insert(std::make_pair("ufd", "application/vnd.ufdl"));
		map.insert(std::make_pair("ez2", "application/vnd.ezpix-album"));
		map.insert(std::make_pair("ez3", "application/vnd.ezpix-package"));
		map.insert(std::make_pair("plf", "application/vnd.pocketlearn"));
		map.insert(std::make_pair("mpga", "audio/mpeg"));
		map.insert(std::make_pair("plc", "application/vnd.mobius.plc"));
		map.insert(std::make_pair("plb", "application/vnd.3gpp.pic-bw-large"));
		map.insert(std::make_pair("twds", "application/vnd.simtech-mindmapper"));
		map.insert(std::make_pair("c4p", "application/vnd.clonk.c4group"));
		map.insert(std::make_pair("nml", "application/vnd.enliven"));
		map.insert(std::make_pair("iges", "model/iges"));
		map.insert(std::make_pair("dcr", "application/x-director"));
		map.insert(std::make_pair("bz", "application/x-bzip"));
		map.insert(std::make_pair("acc", "application/vnd.americandynamics.acc"));
		map.insert(std::make_pair("src", "application/x-wais-source"));
		map.insert(std::make_pair("ppm", "image/x-portable-pixmap"));
		map.insert(std::make_pair("es3", "application/vnd.eszigno3+xml"));
		map.insert(std::make_pair("zip", "application/zip"));
		map.insert(std::make_pair("ace", "application/x-ace-compressed"));
		map.insert(std::make_pair("uvvh", "video/vnd.dece.hd"));
		map.insert(std::make_pair("dd2", "application/vnd.oma.dd2+xml"));
		map.insert(std::make_pair("obd", "application/x-msbinder"));
		map.insert(std::make_pair("cmdf", "chemical/x-cmdf"));
		map.insert(std::make_pair("wmlsc", "application/vnd.wap.wmlscriptc"));
		map.insert(std::make_pair("xap", "application/x-silverlight-app"));
		map.insert(std::make_pair("acu", "application/vnd.acucobol"));
		map.insert(std::make_pair("sru", "application/sru+xml"));
		map.insert(std::make_pair("srx", "application/sparql-results+xml"));
		map.insert(std::make_pair("book", "application/vnd.framemaker"));
		map.insert(std::make_pair("clkk", "application/vnd.crick.clicker.keyboard"));
		map.insert(std::make_pair("ppd", "application/vnd.cups-ppd"));
		map.insert(std::make_pair("tiff", "image/tiff"));
		map.insert(std::make_pair("xyz", "chemical/x-xyz"));
		map.insert(std::make_pair("pcurl", "application/vnd.curl.pcurl"));
		map.insert(std::make_pair("cpp", "text/x-c"));
		map.insert(std::make_pair("nsf", "application/vnd.lotus-notes"));
		map.insert(std::make_pair("clkx", "application/vnd.crick.clicker"));
		map.insert(std::make_pair("weba", "audio/webm"));
		map.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
		map.insert(std::make_pair("clkp", "application/vnd.crick.clicker.palette"));
		map.insert(std::make_pair("dfac", "application/vnd.dreamfactory"));
		map.insert(std::make_pair("clkt", "application/vnd.crick.clicker.template"));
		map.insert(std::make_pair("djvu", "image/vnd.djvu"));
		map.insert(std::make_pair("clkw", "application/vnd.crick.clicker.wordbank"));
		map.insert(std::make_pair("xpi", "application/x-xpinstall"));
		map.insert(std::make_pair("w3d", "application/x-director"));
		map.insert(std::make_pair("xpm", "image/x-xpixmap"));
		map.insert(std::make_pair("fzs", "application/vnd.fuzzysheet"));
		map.insert(std::make_pair("ppam", "application/vnd.ms-powerpoint.addin.macroenabled.12"));
		map.insert(std::make_pair("wad", "application/x-doom"));
		map.insert(std::make_pair("setpay", "application/set-payment-initiation"));
		map.insert(std::make_pair("pic", "image/x-pict"));
		map.insert(std::make_pair("mdb", "application/x-msaccess"));
		map.insert(std::make_pair("xpx", "application/vnd.intercon.formnet"));
		map.insert(std::make_pair("smil", "application/smil+xml"));
		map.insert(std::make_pair("xps", "application/vnd.ms-xpsdocument"));
		map.insert(std::make_pair("xpr", "application/vnd.is-xpr"));
		map.insert(std::make_pair("gim", "application/vnd.groove-identity-message"));
		map.insert(std::make_pair("xpw", "application/vnd.intercon.formnet"));
		map.insert(std::make_pair("sdw", "application/vnd.stardivision.writer"));
		map.insert(std::make_pair("sdp", "application/sdp"));
		map.insert(std::make_pair("ipfix", "application/ipfix"));
		map.insert(std::make_pair("sdd", "application/vnd.stardivision.impress"));
		map.insert(std::make_pair("hh", "text/x-c"));
		map.insert(std::make_pair("sda", "application/vnd.stardivision.draw"));
		map.insert(std::make_pair("atom", "application/atom+xml"));
		map.insert(std::make_pair("pwn", "application/vnd.3m.post-it-notes"));
		map.insert(std::make_pair("mscml", "application/mediaservercontrol+xml"));
		map.insert(std::make_pair("sv4cpio", "application/x-sv4cpio"));
		map.insert(std::make_pair("mathml", "application/mathml+xml"));
		map.insert(std::make_pair("m14", "application/x-msmediaview"));
		map.insert(std::make_pair("lvp", "audio/vnd.lucent.voice"));
		map.insert(std::make_pair("dmg", "application/octet-stream"));
		map.insert(std::make_pair("uu", "text/x-uuencode"));
		map.insert(std::make_pair("mts", "model/vnd.mts"));
		map.insert(std::make_pair("cii", "application/vnd.anser-web-certificate-issue-initiation"));
		map.insert(std::make_pair("sgml", "text/sgml"));
		map.insert(std::make_pair("azf", "application/vnd.airzip.filesecure.azf"));
		map.insert(std::make_pair("cil", "application/vnd.ms-artgalry"));
		map.insert(std::make_pair("dms", "application/octet-stream"));
		map.insert(std::make_pair("clp", "application/x-msclip"));
		map.insert(std::make_pair("mads", "application/mads+xml"));
		map.insert(std::make_pair("svg", "image/svg+xml"));
		map.insert(std::make_pair("utz", "application/vnd.uiq.theme"));
		map.insert(std::make_pair("vsd", "application/vnd.visio"));
		map.insert(std::make_pair("xwd", "image/x-xwindowdump"));
		map.insert(std::make_pair("azs", "application/vnd.airzip.filesecure.azs"));
		map.insert(std::make_pair("azw", "application/vnd.amazon.ebook"));
		map.insert(std::make_pair("ac", "application/pkix-attr-cert"));
		map.insert(std::make_pair("sxc", "application/vnd.sun.xml.calc"));
		map.insert(std::make_pair("sxd", "application/vnd.sun.xml.draw"));
		map.insert(std::make_pair("p7s", "application/pkcs7-signature"));
		map.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
		map.insert(std::make_pair("ai", "application/postscript"));
		map.insert(std::make_pair("sxi", "application/vnd.sun.xml.impress"));
		map.insert(std::make_pair("grxml", "application/srgs+xml"));
		map.insert(std::make_pair("sxm", "application/vnd.sun.xml.math"));
		map.insert(std::make_pair("dssc", "application/dssc+der"));
		map.insert(std::make_pair("ppsm", "application/vnd.ms-powerpoint.slideshow.macroenabled.12"));
		map.insert(std::make_pair("fly", "text/vnd.fly"));
		map.insert(std::make_pair("p7b", "application/x-pkcs7-certificates"));
		map.insert(std::make_pair("p7c", "application/pkcs7-mime"));
		map.insert(std::make_pair("aw", "application/applixware"));
		map.insert(std::make_pair("sxw", "application/vnd.sun.xml.writer"));
		map.insert(std::make_pair("nnw", "application/vnd.noblenet-web"));
		map.insert(std::make_pair("curl", "text/vnd.curl"));
		map.insert(std::make_pair("p7m", "application/pkcs7-mime"));
		map.insert(std::make_pair("nns", "application/vnd.noblenet-sealer"));
		map.insert(std::make_pair("uoml", "application/vnd.uoml+xml"));
		map.insert(std::make_pair("grv", "application/vnd.groove-injector"));
		map.insert(std::make_pair("igs", "model/iges"));
		map.insert(std::make_pair("hps", "application/vnd.hp-hps"));
		map.insert(std::make_pair("nb", "application/mathematica"));
		map.insert(std::make_pair("nc", "application/x-netcdf"));
		map.insert(std::make_pair("igx", "application/vnd.micrografx.igx"));
		map.insert(std::make_pair("application", "application/x-ms-application"));
		map.insert(std::make_pair("h261", "video/h261"));
		map.insert(std::make_pair("gre", "application/vnd.geometry-explorer"));
		map.insert(std::make_pair("oas", "application/vnd.fujitsu.oasys"));
		map.insert(std::make_pair("fst", "image/vnd.fst"));
		map.insert(std::make_pair("igm", "application/vnd.insors.igm"));
		map.insert(std::make_pair("igl", "application/vnd.igloader"));
		map.insert(std::make_pair("vrml", "model/vrml"));
		map.insert(std::make_pair("bmi", "application/vnd.bmi"));
		map.insert(std::make_pair("btif", "image/prs.btif"));
		map.insert(std::make_pair("ssml", "application/ssml+xml"));
		map.insert(std::make_pair("link66", "application/vnd.route66.link66+xml"));
		map.insert(std::make_pair("au", "audio/basic"));
		map.insert(std::make_pair("fsc", "application/vnd.fsc.weblaunch"));
		map.insert(std::make_pair("dvi", "application/x-dvi"));
		map.insert(std::make_pair("for", "text/x-fortran"));
		map.insert(std::make_pair("cpt", "application/mac-compactpro"));
		map.insert(std::make_pair("svd", "application/vnd.svd"));
		map.insert(std::make_pair("vcg", "application/vnd.groove-vcard"));
		map.insert(std::make_pair("svc", "application/vnd.dvb.service"));
		map.insert(std::make_pair("portpkg", "application/vnd.macports.portpkg"));
		map.insert(std::make_pair("eol", "audio/vnd.digital-winds"));
		map.insert(std::make_pair("xbm", "image/x-xbitmap"));
		map.insert(std::make_pair("iif", "application/vnd.shana.informed.interchange"));
		map.insert(std::make_pair("oa3", "application/vnd.fujitsu.oasys3"));
		map.insert(std::make_pair("oa2", "application/vnd.fujitsu.oasys2"));
		map.insert(std::make_pair("eot", "application/vnd.ms-fontobject"));
		map.insert(std::make_pair("onetoc", "application/onenote"));
		map.insert(std::make_pair("xbd", "application/vnd.fujixerox.docuworks.binder"));
		map.insert(std::make_pair("mrc", "application/marc"));
		map.insert(std::make_pair("sbml", "application/sbml+xml"));
		map.insert(std::make_pair("m2v", "video/mpeg"));
	}

	SCP_string getExtension(const SCP_string& other)
	{
		size_t slash = other.find_last_of(vfspp::DirectorySeparatorStr);
		size_t dot = other.find_last_of(".");

		if (slash > dot)
		{
			return SCP_string();
		}
		else
		{
			if (dot != SCP_string::npos)
			{
				return other.substr(dot + 1);
			}
			else
			{
				return SCP_string();
			}
		}
	}
}

namespace chromium
{

	class CFileHandlerFactory : public CefSchemeHandlerFactory
	{
	private:
		boost::unordered_map<SCP_string, SCP_string> mimeTypeMap;

	public:
		CFileHandlerFactory()
		{
			fillMimeMap(mimeTypeMap);
		}

		virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override;

		inline SCP_string getMimeType(const SCP_string& extension)
		{
			boost::unordered_map<SCP_string, SCP_string>::iterator iter = mimeTypeMap.find(extension);

			if (iter == mimeTypeMap.end())
			{
				// We don't know this extension...
				return "application/octet-stream";
			}
			else
			{
				return iter->second;
			}
		}

		IMPLEMENT_REFCOUNTING(CFileHandlerFactory);
	};
	
	class CFileHandler : public CefReadHandler
	{
	private:
		cfile::FileHandle* handle;

	public:
		CFileHandler(cfile::FileHandle* handleIn) : handle(handleIn) {}

		~CFileHandler()
		{
			if (handle != nullptr)
			{
				cfile::close(handle);
			}
		}

		virtual size_t Read(void* ptr, size_t size, size_t n)
		{
			return cfile::read(ptr, size, n, handle);
		}

		virtual int Seek(int64 offset, int whence)
		{
			cfile::SeekMode mode;
			switch (whence)
			{
			case SEEK_CUR:
				mode = cfile::SEEK_MODE_CUR;
				break;
			case SEEK_END:
				mode = cfile::SEEK_MODE_END;
				break;
			case SEEK_SET:
				mode = cfile::SEEK_MODE_SET;
				break;
			default:
				mode = cfile::SEEK_MODE_SET;
				break;
			}

			return cfile::seek(handle, (int)offset, mode);
		}

		virtual int64 Tell()
		{
			return cfile::tell(handle);
		}

		virtual int Eof()
		{
			return cfile::eof(handle) ? 1 : 0;
		}

		IMPLEMENT_REFCOUNTING(CFileHandler);
	};

	CefRefPtr<CefResourceHandler> CFileHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
	{
		CefURLParts parts;
		CefParseURL(request->GetURL(), parts);

		std::string path = CefString(&parts.path).ToString();

		SCP_string filePath = vfspp::util::normalizePath(path).c_str();

		cfile::FileHandle *handle = cfile::open(filePath);

		if (handle == nullptr)
		{
			return nullptr;
		}
		else
		{
			SCP_string mimeType = getMimeType(getExtension(filePath));
			// Return a new resource handler instance to handle the request.
			return new CefStreamResourceHandler(mimeType.c_str(), CefStreamReader::CreateForHandler(new CFileHandler(handle)));
		}
	}

	void ApplicationImpl::AddCallbackName(CefString const& name)
	{
		boost::lock_guard<boost::mutex> guard(mCallbacknamesLock);

		mCallbackNames.push_back(name);
	}

	void ApplicationImpl::RemoveCallback(CefString const& name)
	{
		boost::lock_guard<boost::mutex> guard(mCallbacknamesLock);

		mCallbackNames.erase(std::remove(mCallbackNames.begin(), mCallbackNames.end(), name));
	}

	void ApplicationImpl::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
	{
		// Register custom schemes here
	}

	void ApplicationImpl::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
	{
		boost::lock_guard<boost::mutex> guard(mCallbacknamesLock);

		CefRefPtr<CefListValue> callbackNames = CefListValue::Create();
		callbackNames->SetSize(mCallbackNames.size());

		int i = 0;
		for (auto& name : mCallbackNames)
		{
			callbackNames->SetString(i, name);

			++i;
		}

		extra_info->SetList(0, callbackNames);

		SCP_vector<CefString> apiFunctions;
		jsapi::getFunctionNames(apiFunctions);

		CefRefPtr<CefListValue> extraFunctions = CefListValue::Create();
		extraFunctions->SetSize(apiFunctions.size());

		i = 0;
		for (auto& name : apiFunctions)
		{
			extraFunctions->SetString(i, name);

			++i;
		}

		extra_info->SetList(1, extraFunctions);
	}

	void ApplicationImpl::OnContextInitialized()
	{
		CefRegisterSchemeHandlerFactory("http", "fso", new CFileHandlerFactory());
	}
}
