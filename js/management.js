Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('management_form', 'submit', 
        function(e) {
            if (!confirm('更新します。よろしいですか？')) {
                Event.stop(e);
                return false;
            }
            return true;
        });
}
