Public Class Form8
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Dim oForm As New Form1()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        System.Diagnostics.Process.Start("http://adeleted.blogspot.com")
    End Sub
    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
	
    End Sub

    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        System.Diagnostics.Process.Start("http://adeleted.blogspot.com/p/blog-page.html")
    End Sub
End Class