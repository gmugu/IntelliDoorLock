#ifndef __INDEX_PAGE_H
#define __INDEX_PAGE_H
#define INDEX_HTML  "<html>"\
"			</div>"\
"			<div class=\"mobile-view\" align=\"center\">"\
"				<table border='1'>"\
"					<thead>"\
"						<th>ID</th>"\
"						<th>名称</th>"\
"						<th>标识码</th>"\
"						<th colspan='2'>操作</th>"\
"					</thead>"\
"					<tbody id='device_table'>"\
"						<tr>"\
"							<td>sdgsdg </td>"\
"							<td>sdgsdg </td>"\
"							<td>sdgdgsdg</td>"\
"							<th><a href=''>重命名</a></th>"\
"							<th><a href=''>删除</a></th>"\
"						</tr>"\
"						<tr>"\
"							<td>sdgdfsdg </td>"\
"							<td>sdgdfsdg </td>"\
"							<td>dg</td>"\
"							<th><a href=''>重命名</a></th>"\
"							<th><a href=''>删除</a></th>"\
"						</tr>"\
"					</tbody>"\
"//visitor block end"\
"//cloud service begin"\
"function init_cloud_view () {"\
"	"\
"}"\
"//cloud service end"\
"//admin block begin"\
"function admin_save(){"\
"}"\
"function reboot () {"\
"	"\
"}"\
"//admin block end"\
"function init () {"\
"	// ajaxGetModel(function (amodel) {"\
"		// model = amodel;"\
"		init_dk_view();"\
"		init_fingerprint_view();"\
"		init_mobile_view();"\
"		init_visitor_view();"\
"		init_cloud_view();"\
"	// });"\
"}"\
"init();"\
"</script>"

#endif
