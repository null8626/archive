Public Class Form1
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim oForm As New Form2()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        Dim oForm As New Form8()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
End Class
