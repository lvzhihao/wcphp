<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('wordcheck')) {
	dl('wordcheck.' . PHP_SHLIB_SUFFIX);
}
$module = 'wordcheck';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";
$function = 'wordcheck_filter';
if (extension_loaded($module)) {
	$str = $function($module);
} else {
	$str = "Module $module is not compiled into PHP";
}
echo "$str\n";

$output = array();

wordcheck_set_replace_op("#");
wordcheck_reset();

$outstring = wordcheck_filter("文鲜明上海商派网络科技有限公司（以下简称，ShopEx）成立于2002年，是国内最大的电子商务软件及服务提供商。ShopEx长期专注于电子商务软件的研发及相关解决方案与服务的提供。多年来，ShopEx始终秉承“做最好的电子商务软件和服务”之理念，引领中国电子商务行业技术的发展方向。

经过近十年积累，ShopEx形成了一支宋杰业内规模最大、最具实力、最富经验的专业团队，潜心钻研具有自主核心技术和知识产权的电子商务软件及贴合用户需求，独具行业特色的服务产品。主要产品ShopEx、Ecshop、分销王、店掌柜等网店系统是当前国内最受用户青睐的电子商务软件。

ShopEx通过与阿里巴巴，淘宝，支付宝，腾讯，隐藏在共和国拍拍，财付通，百度，乐酷天，中国移动，中国电信，中国银联，google，ebay，paypal，用友，金蝶，新浪 等众多业内外知名企业建立战略合作联盟，为用户创造更优异的产品及更高品质的服务。尊贵的仁布其 隐藏在共和国
宋杰
凭借对电子商务行业的深刻理解和丰富的实战经验，ShopEx获得了23个行业、8种业态、80万用户的肯定，且已成为国内80%电子商务年营业额过亿企业的共同选择。ShopEx坚守品质，追求卓越，将为更多企业探索电子商务的成功之宋杰道，促进中国电子商务行业的快速发展。隐藏在共和国", $output);

var_dump($outstring);

var_dump($output);

?>
